/**
 * Copyright (c) 2011 Nokia Corporation.
 * All rights reserved.
 *
 * Part of the Qt GameEnabler.
 *
 * For the applicable distribution terms see the license text file included in
 * the distribution.
 */

#include <QtGlobal>
#include <QDebug>
#include <QTime>
#include <stdlib.h>
#include <memory.h>

#include "trace.h"
#include "vorbisdecoder.h"
#include "oggpage.h"
#include "oggconstants.h"

using namespace GE;

const int maxblocksize = 512 * 1024;

VorbisDecoder::VorbisDecoder(bool cached, QObject *parent) :
    QObject(parent),
    m_file(this),
    m_pageList(0),
    m_pos(0),
    m_cached(cached),
    m_cache(NULL),
    m_vorbis(NULL),
    m_decSamples(NULL),
    m_decSamplesLen(0),
    m_maxDecSamplesLen(0),
    m_readData(NULL),
    m_decodedLength(0)
{
}

VorbisDecoder::~VorbisDecoder()
{
    m_file.close();

    OggPage *p = m_pageList;
    while (p) {
        OggPage *pn = p->m_next;
        delete p;
        p = pn;
    }

    vorbisUninit();

    delete m_cache;
    delete m_decSamples;
    delete m_readData;
}

bool VorbisDecoder::load(QString &filename)
{
    m_file.setFileName(filename);

    if (!m_file.open(QIODevice::ReadOnly)) {
        DEBUG_INFO("Failed to open file" << filename);
        return false;
    }

    if (m_cached) {
        m_length = m_file.size();
        m_pos = 0;
        m_cache = (unsigned char*)malloc(m_length);
        qint64 len = m_file.read((char*)m_cache, m_length);
        if (len != m_length) {
            DEBUG_INFO("Failed to read file" << filename << "read"
                <<  len << "expected" << m_length);
            return false;
        }
        m_file.close();
    }

    if (!scan())
        return false;

	if (vorbisInit() < 0) {
	    DEBUG_INFO("Failed to initialize vorbis decoder");
	    return false;
    }

    return true;
}

bool VorbisDecoder::scan()
{
    // Read the first page. It must be the beginning-of-stream page.
    m_pageList = readPageHeader();
    if (!m_pageList || !m_pageList->m_headerType & OggBOS) {
        DEBUG_INFO("Failed to read BOS page");
        delete m_pageList;
        m_pageList = NULL;
        return false;
    }
    OggPage *page = m_pageList;

    // Read all the pages until eof or end-of-stream page reachead
    while (!(page->m_headerType & OggEOS) &&
            (page->m_next = readPageHeader()))
    {
        page->m_next->m_prev = page;
        page = page->m_next;
        //page->dump();
        seek(tell() + page->m_dataLength);
    }

    m_decodedLength = page->m_granulePosition;
    m_currentPage = m_pageList;
    // TODO: fill this properly (the stream does not always start from 0)
    m_granuleOffset = 0;

    return true;
}

const OggPage *VorbisDecoder::seekPage(quint64 samplePos)
{
    OggPage *p = m_pageList;
    if (!p)
        return NULL;

    do {
        if (p->m_granulePosition &&
            p->m_granulePosition - m_granuleOffset > samplePos) {
            m_currentPage = p;
            return p;
        }
    } while ((p = p->m_next));

    return m_currentPage;
}

const OggPage *VorbisDecoder::nextPage()
{
    if (m_currentPage->m_next)
        m_currentPage = m_currentPage->m_next;

    return m_currentPage;
}

const OggPage *VorbisDecoder::prevPage()
{
    if (m_currentPage->m_prev)
        m_currentPage = m_currentPage->m_prev;

    return m_currentPage;
}

const OggPage *VorbisDecoder::firstPage()
{
    m_currentPage = m_pageList;
    return m_currentPage;
}

const OggPage *VorbisDecoder::firstAudioPage()
{
    OggPage *page = m_pageList;
    // 'The granule position of these first pages containing only headers is zero.'
    while (page->m_granulePosition == 0) {
        page = page->m_next;
        if (!page)
            return NULL;
    }
    m_currentPage = page;
    return m_currentPage;
}

const stb_vorbis_info *VorbisDecoder::fileInfo()
{
    return &m_info;
}

OggPage *VorbisDecoder::readPageHeader()
{
    int len;

    if (!m_file.isOpen() && !m_cache)
        return NULL;

    OggPage *page = new OggPage();
    if (!page) {
        DEBUG_INFO("Failed to allocate a page");
        return NULL;
    }

    page->m_pageStartPos = tell();

    len = read((unsigned char*)page, OggPageHeaderLen, -1);
    if (!len) {
        DEBUG_INFO("Failed to read a page");
        delete page;
        return NULL;
    }

    if (page->m_capturePattern != OggCapturePatternMagic) {
        DEBUG_INFO("Capture pattern mismatch");
        delete page;
        return NULL;
    }

    // First page does not contain segments at all. It contains only an Vorbis
    // identification header.
    if (page->m_headerType & OggBOS) {
        seek(OggFirstPageLen);
        page->m_segments = NULL;
        page->m_dataLength = 0;
        page->m_pageLength = OggFirstPageLen;
    } else {
        page->m_segments = (unsigned char*)malloc(page->m_pageSegments);
        if (!page->m_segments) {
            DEBUG_INFO("Failed to allocate page segments");
            delete page;
            return NULL;
        }
        len = read(page->m_segments, page->m_pageSegments, -1);
        if (len != page->m_pageSegments) {
            DEBUG_INFO("Read" << len << "segment lengths, expected"
                << page->m_pageSegments);
            delete page;
            return NULL;
        }

        for (int t = 0; t < page->m_pageSegments; t++) {
            page->m_dataLength += page->m_segments[t];
        }
    }
    page->m_dataStartPos = tell();
    page->m_pageLength = page->m_dataStartPos - page->m_pageStartPos +
        page->m_dataLength;

    return page;
}

unsigned char *VorbisDecoder::readPage(const OggPage *page)
{
    if (!page) {
        DEBUG_INFO("NULL page given!");
        return NULL;
    }

    unsigned char *data = (unsigned char*)malloc(page->m_pageLength);
    if (!data)
        return NULL;

    unsigned int len = read(data, page->m_pageLength, page->m_pageStartPos);
    if (len != page->m_pageLength) {
        DEBUG_INFO("Read" << len << "bytes, expected" << page->m_pageLength);
        free(data);
        return NULL;
    }

    return data;
}

int VorbisDecoder::read(unsigned char *buf, int len, int pos)
{
    if (m_cached) {
        if (!m_cache)
            return -1;
        if (pos >=0)
            seek(pos);
        int l = qMin(len, m_length - m_pos);
        memcpy(buf, &m_cache[m_pos], l);
        m_pos += l;
        return l;
    } else {
        if (!m_file.isOpen())
            return -1;
        if (pos >= 0) {
            if (!seek(pos))
                return -1;
        }
        return m_file.read((char*)buf, len);
    }
}

bool VorbisDecoder::seek(int pos)
{
    if (m_cached) {
        if (!m_cache)
            return false;
        if (pos >= m_length || pos < 0)
            return false;
        m_pos = pos;
        return m_pos;
    } else {
        if (!m_file.isOpen())
            return false;
        return m_file.seek(pos);
    }
}

int VorbisDecoder::tell()
{
    if (m_cached) {
        if (!m_cache)
            return -1;
        return m_pos;
    } else {
        if (!m_file.isOpen())
            return -1;
        return m_file.pos();
    }
}

unsigned char *VorbisDecoder::readHeaderPages(int *len)
{
    OggPage *page = m_pageList;
    if (!page)
        return NULL;

    *len = 0;
    // 'The granule position of these first pages containing only headers is zero.'
    while (page->m_granulePosition == 0) {
        *len += page->m_pageLength;
        page = page->m_next;
        if (!page)
            return NULL;
    }

    unsigned char *data = (unsigned char*)malloc(*len);
    if (!data)
        return NULL;

    int rlen = read(data, *len, 0);
    if (rlen != *len) {
        DEBUG_INFO("Read" << rlen << "bytes, expected" << *len);
        free(data);
        return NULL;
    }

    return data;
}

unsigned char *VorbisDecoder::decodeAll(unsigned int *len)
{
    if (!m_vorbis)
        return NULL;

    unsigned char *outBuf = (unsigned char*)malloc(m_decodedLength);
    if (!outBuf) {
        DEBUG_INFO("Failed to allocate" << m_decodedLength << "bytes!");
        return NULL;
    }
    *len = 0;

    m_readPos = firstAudioPage()->m_pageStartPos;
    int rlen = 0;
    while ((rlen = read(m_readData, 65536, m_readPos)) > 0) {
        int leftover = 0;
        if (!vorbisDecode(m_readData, rlen, &m_decSamplesLen, &leftover)
            || !m_decSamplesLen) {
            DEBUG_INFO("vorbisDecode failed");
            return false;
        }
        m_readPos += rlen - leftover;

        if (*len + m_decSamplesLen > m_decodedLength) {
	        // Should not happen
        	outBuf = (unsigned char*)realloc(outBuf, m_decSamplesLen + *len);
        	if (!outBuf) {
                DEBUG_INFO("Failed to allocate" << m_decSamplesLen + *len
                    << "bytes!");
                return NULL;
            }
    	}
    	memcpy(&outBuf[*len], m_decSamples, m_decSamplesLen);
    	*len += m_decSamplesLen;
	}

	return outBuf;
}

unsigned short VorbisDecoder::at(quint64 pos)
{
    if (!m_decSamplesLen || pos < m_decodedDataStart ||
        pos > m_decodedDataEnd) {
        if (!vorbisSeek(pos / 2))
            return 0;
    }

    return m_decSamples[pos - m_decodedDataStart];
}

int VorbisDecoder::vorbisInit()
{
    int used = 0, error = 0;
    int len;

    unsigned char *data = readHeaderPages(&len);
    if (!data)
        return -1;

    m_vorbis = stb_vorbis_open_pushdata((unsigned char*)data, len, &used,
        &error, NULL);

    DEBUG_INFO("stb vorbis init complete, used" << used
        << "bytes, error" << error);

    if (!m_vorbis)
        return -error;

    memset(&m_info, 0, sizeof(stb_vorbis_info));
    m_info = stb_vorbis_get_info(m_vorbis);

    DEBUG_INFO("   sample_rate:" << m_info.sample_rate);
    DEBUG_INFO("      channels:" << m_info.channels);
    DEBUG_INFO("max_frame_size:" << m_info.max_frame_size);

	free(data);
	const OggPage *page = firstAudioPage();
	m_readPos = page->m_pageStartPos;
    m_readData = (unsigned char *)malloc(maxblocksize);

    return used;
}

void VorbisDecoder::vorbisUninit()
{
    if (m_vorbis) {
        stb_vorbis_close(m_vorbis);
        m_vorbis = NULL;
    }

    free(m_readData);
    m_readData = NULL;
    free(m_decSamples);
    m_decSamples = NULL;
    m_decSamplesLen = 0;
}

short *VorbisDecoder::vorbisDecode(unsigned char *data, int len, int *outLen,
    int *leftover)
{
    float **outputs;
    int channels, samplecount, t, i;
    int outPos = 0;
    unsigned char *p = data;

	if (!m_decSamples) {
        m_maxDecSamplesLen = maxblocksize;
		m_decSamples = (short*)malloc(sizeof(short) * m_maxDecSamplesLen);
	}
    m_decSamplesLen = 0;

    while (len) {
        int used = stb_vorbis_decode_frame_pushdata(m_vorbis, (unsigned char*)p,
            len, &channels, &outputs, &samplecount);
        if (!used && !samplecount)
            break;
        len -= used;
        p += used;
        if (outPos + samplecount * channels > m_maxDecSamplesLen) {
            m_maxDecSamplesLen += maxblocksize;
            m_decSamples = (short*)realloc(m_decSamples,
                m_maxDecSamplesLen * sizeof(short));
        }

        for (t = 0; t < samplecount; t++) {
            for (i = 0; i < channels; i++) {
                // TODO: Values are not clamped to -1...1 by stb vorbis
                m_decSamples[outPos++] = (short)(outputs[i][t] * 30000.0f);
            }
        }
    }
    *leftover = len;
    *outLen = outPos * sizeof(short);
    return m_decSamples;
}

bool VorbisDecoder::vorbisFlush()
{
	if (!m_vorbis)
		return false;
    stb_vorbis_flush_pushdata(m_vorbis);
    return true;
}

bool VorbisDecoder::vorbisDecodeCurrent()
{
    m_decSamplesLen = 0;
    OggPage *startPage = m_currentPage->m_prev;
    OggPage *endPage = m_currentPage;
    OggPage *p = startPage;
    do {
        if (p->m_granulePosition != startPage->m_granulePosition &&
            p->m_granulePosition != (quint64)-1) {
            startPage = p;
            break;
        }
    } while((p = p->m_prev));

    m_readPos = startPage->m_pageStartPos;
    int lenToRead = endPage->m_pageStartPos - startPage->m_pageStartPos +
        endPage->m_pageLength;

    int rlen = read(m_readData, lenToRead, m_readPos);
    if (!rlen)
        return false;

    int leftover = 0;
    if (!vorbisDecode(m_readData, rlen, &m_decSamplesLen, &leftover)
        || !m_decSamplesLen) {
        DEBUG_INFO("vorbisDecode failed");
        return false;
    }
    m_readPos += rlen - leftover;

    int decPos = m_decSamplesLen / m_info.channels / sizeof(short);
    if (endPage->m_granulePosition < decPos) {
        m_decodedDataStart = 0;
    } else {
        m_decodedDataStart = (endPage->m_granulePosition - decPos -
            m_granuleOffset) * 2;
    }
    m_decodedDataEnd = (endPage->m_granulePosition - m_granuleOffset) * 2;
    m_currentPage = endPage;
    return true;
}

bool VorbisDecoder::vorbisSeekRelative(qint64 offset)
{
    if (!m_vorbis)
        return false;

    int current = stb_vorbis_get_sample_offset(m_vorbis);
    if (current == -1) {
        DEBUG_INFO("no offset yet");
        return false;
    }

    const OggPage *page = seekPage(current + offset);
    if (!page)
        return false;

    if (!vorbisFlush())
		return false;

	if (!vorbisDecodeCurrent())
        return false;

    return true;
}

bool VorbisDecoder::vorbisSeek(qint64 pos)
{
    const OggPage *page = seekPage(pos);
    if (!page)
        return false;

    if (!vorbisFlush())
		return false;

	if (!vorbisDecodeCurrent())
        return false;

    return true;
}
