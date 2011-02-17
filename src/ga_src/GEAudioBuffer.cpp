/*
 * Copyright  2011 Nokia Corporation.
 */

#include <QtCore>
#include <math.h>
#include "GEAudioBuffer.h"


using namespace GE;


// Header for "Wav"-data
class SWavHeader {
public:
    SWavHeader()
    {
        memset(this, 0, sizeof(SWavHeader));
    }

    char chunkID[4];
    unsigned int chunkSize;
    char format[4];

    unsigned char subchunk1id[4];
    unsigned int subchunk1size;
    unsigned short audioFormat;
    unsigned short nofChannels;
    unsigned int sampleRate;
    unsigned int byteRate;

    unsigned short blockAlign;
    unsigned short bitsPerSample;

    unsigned char subchunk2id[4];
    unsigned int subchunk2size;
};


AudioBuffer::AudioBuffer()
{
    m_data = 0;
    m_dataLength = 0;
    m_sampleFunction = 0;
}


AudioBuffer::~AudioBuffer()
{
    reallocate(0);
}


void AudioBuffer::reallocate(int length)
{
    if (m_data)
        delete [] ((char*)m_data);

    m_dataLength = length;

    if (m_dataLength > 0)
        m_data = new char[m_dataLength];
    else
        m_data = 0;
}


AudioBuffer* AudioBuffer::loadWav(const QString &fileName, QString *errorString)
{
    QFile file(fileName);
    return loadWav(file, errorString);
}


AudioBuffer* AudioBuffer::loadWav(FILE *wavFile, QString *errorString)
{
    QFile tempFile;
    tempFile.open(wavFile, QIODevice::ReadOnly);
    return loadWav(tempFile, errorString);
}


AudioBuffer* AudioBuffer::loadWav(QFile &wavFile, QString *errorString)
{
    if (wavFile.open(QIODevice::ReadOnly) == false) {
        if(errorString)
            *errorString = QString("Permission problem");
        return 0;
    }

    SWavHeader header;
    AudioBuffer *rval = 0;

    try {
        wavFile.read(header.chunkID, 4);
        if (header.chunkID[0] != 'R' || header.chunkID[1] != 'I' ||
                header.chunkID[2] != 'F' || header.chunkID[3] != 'F') {
            // Incorrect header
            if(errorString)
                *errorString = QString("Incorrect header");
            return 0;
        }

        wavFile.read((char*)&header.chunkSize, 4);
        wavFile.read((char*)&header.format, 4);

        if (header.format[0] != 'W' || header.format[1] != 'A' ||
                header.format[2] != 'V' || header.format[3] != 'E') {
            // Incorrect header
            if(errorString)
                *errorString = QString("Incorrect header");
            return 0;
        }

        wavFile.read((char*)&header.subchunk1id, 4);
        if (header.subchunk1id[0] != 'f' || header.subchunk1id[1] != 'm' ||
                header.subchunk1id[2] != 't' || header.subchunk1id[3] != ' ') {
            // Incorrect header
            if(errorString)
                *errorString = QString("Incorrect header");
            return 0;
        }

        wavFile.read((char*)&header.subchunk1size, 4);
        wavFile.read((char*)&header.audioFormat, 2);
        wavFile.read((char*)&header.nofChannels, 2);
        wavFile.read((char*)&header.sampleRate, 4);
        wavFile.read((char*)&header.byteRate, 4);
        wavFile.read((char*)&header.blockAlign, 2);
        wavFile.read((char*)&header.bitsPerSample, 2);

        while (1) {
            if (wavFile.read((char*)&header.subchunk2id, 4 ) != 4) {
                if(errorString)
                    *errorString = QString("Incorrect header");
                return 0;
            }
            if (wavFile.read((char*)&header.subchunk2size, 4 ) != 4) {
                if(errorString)
                    *errorString = QString("Incorrect header");
                return 0;
            }
            if (header.subchunk2id[0]=='d' && header.subchunk2id[1] == 'a' &&
                    header.subchunk2id[2]=='t' && header.subchunk2id[3] == 'a')
                break; // found the data, chunk
            // this was not the data-chunk. skip it
            if (header.subchunk2size < 1) {
                if(errorString)
                    *errorString = QString("Incorrect header");
                return 0;
            }
            wavFile.seek(wavFile.pos() + header.subchunk2size);
        }

        // the data follows.
        if (header.subchunk2size < 1) {
            if(errorString)
                *errorString = QString("Incorrect header");
            return 0;
        }

        rval = new AudioBuffer;
        rval->m_nofChannels = header.nofChannels;
        rval->m_bitsPerSample = header.bitsPerSample;
        rval->m_samplesPerSec = header.sampleRate;
        rval->m_signedData = 0; // where to know this?
        rval->reallocate(header.subchunk2size);

        wavFile.read((char*)rval->m_data, header.subchunk2size);

        // choose a good sampling function.
        rval->m_sampleFunction = 0;
        if (rval->m_nofChannels == 1) {
            if (rval->m_bitsPerSample == 8)
                rval->m_sampleFunction = sampleFunction8bitMono;
            if (rval->m_bitsPerSample == 16)
                rval->m_sampleFunction = sampleFunction16bitMono;
            if (rval->m_bitsPerSample == 32)
                rval->m_sampleFunction = sampleFunction32bitMono;
        }
        else {
            if (rval->m_bitsPerSample == 8)
                rval->m_sampleFunction = sampleFunction8bitStereo;
            if (rval->m_bitsPerSample == 16)
                rval->m_sampleFunction = sampleFunction16bitStereo;
            if (rval->m_bitsPerSample == 32)
                rval->m_sampleFunction = sampleFunction32bitStereo;
        }

        if(rval->m_sampleFunction == NULL) {
            // unknown bit rate
            delete rval;
            rval = 0;

            if(errorString)
                *errorString = QString("Unsupported bit rate");
            return 0;
        }
    }
    catch(...) {
        if(rval != 0) {
            delete rval;
            rval = 0;
        }

        if(errorString)
            *errorString = QString("Out of memory");
        return 0;
    }

    return rval;
}


// mix to  mono-versions.
AUDIO_SAMPLE_TYPE AudioBuffer::sampleFunction8bitMono(AudioBuffer *abuffer,
                                                      int pos,
                                                      int /*channel*/)
{
    return (AUDIO_SAMPLE_TYPE)(((quint8*)(abuffer->m_data))[pos] -
                               128) << 8;
}


AUDIO_SAMPLE_TYPE AudioBuffer::sampleFunction16bitMono(AudioBuffer *abuffer,
                                                       int pos,
                                                       int /*channel*/)
{
    return (AUDIO_SAMPLE_TYPE)(((quint16*)(abuffer->m_data))[pos]);
}


AUDIO_SAMPLE_TYPE AudioBuffer::sampleFunction32bitMono(AudioBuffer *abuffer,
                                                       int pos,
                                                       int /*channel*/)
{
    return (((float*)(abuffer->m_data))[pos * abuffer->m_nofChannels]) *
            65536.0f / 2.0f;
}

// mix to stereo-versions.
AUDIO_SAMPLE_TYPE AudioBuffer::sampleFunction8bitStereo(AudioBuffer *abuffer,
                                                        int pos,
                                                        int channel)
{
    return ((AUDIO_SAMPLE_TYPE)
            (((quint8*)(abuffer->m_data))[pos * abuffer->m_nofChannels +
             channel]) << 8);
}


AUDIO_SAMPLE_TYPE AudioBuffer::sampleFunction16bitStereo(AudioBuffer *abuffer,
                                                         int pos,
                                                         int channel)
{
    return (AUDIO_SAMPLE_TYPE)
            (((quint16*)(abuffer->m_data))[pos * abuffer->m_nofChannels +
             channel]);
}


AUDIO_SAMPLE_TYPE AudioBuffer::sampleFunction32bitStereo(AudioBuffer *abuffer,
                                                         int pos,
                                                         int channel)
{
    return (((float*)(abuffer->m_data))[pos * abuffer->m_nofChannels +
            channel]) * 65536.0f / 2.0f;
}


AudioBufferPlayInstance *AudioBuffer::playWithMixer(AudioMixer &mixer)
{
    AudioBufferPlayInstance *i =
            (AudioBufferPlayInstance*)mixer.addAudioSource(
                new AudioBufferPlayInstance(this));
    return i;
}


AudioBufferPlayInstance::AudioBufferPlayInstance()
{
    m_fixedPos = 0;
    m_fixedInc = 0;
    m_buffer = 0;
    m_fixedLeftVolume = 4096;
    m_fixedRightVolume = 4096;
    m_destroyWhenFinished = true;
    m_finished = false;
}


AudioBufferPlayInstance::AudioBufferPlayInstance(
    AudioBuffer *startPlaying)
{
    m_fixedPos = 0;
    m_fixedInc = 0;
    m_fixedLeftVolume = 4096;
    m_fixedRightVolume = 4096;
    m_destroyWhenFinished = true;
    m_finished = false;
    playBuffer( startPlaying, 1.0f, 1.0f );
}


void AudioBufferPlayInstance::playBuffer(AudioBuffer *startPlaying,
                                         float volume,
                                         float speed,
                                         int loopTimes)
{
    m_buffer = startPlaying;
    m_fixedLeftVolume = (int)(4096.0f*volume);
    m_fixedRightVolume = m_fixedLeftVolume;
    m_fixedPos = 0;
    setSpeed( speed );
    m_loopTimes = loopTimes;
}


AudioBufferPlayInstance::~AudioBufferPlayInstance()
{
}


void AudioBufferPlayInstance::stop()
{
    m_buffer = 0;
    m_finished = true;
}


void AudioBufferPlayInstance::setSpeed(float speed)
{
    if (!m_buffer)
        return;
    m_fixedInc = (int)(((float)m_buffer->samplesPerSec() * 4096.0f*speed) /
                       (float)AUDIO_FREQUENCY);
}


void AudioBufferPlayInstance::setLeftVolume(float vol)
{
    m_fixedLeftVolume = (int)(4096.0f * vol);
}


void AudioBufferPlayInstance::setRightVolume(float vol)
{
    m_fixedRightVolume = (int)(4096.0f * vol);
}


bool AudioBufferPlayInstance::canBeDestroyed()
{
    if (m_finished == true && m_destroyWhenFinished == true)
        return true;
    else
        return false;
}


// Doesnt do any bound-checking, must be checked before called.
int AudioBufferPlayInstance::mixBlock(AUDIO_SAMPLE_TYPE *target,
                                      int samplesToMix)
{
    SAMPLE_FUNCTION_TYPE sampleFunction = m_buffer->sampleFunction();
    if (!sampleFunction)
        return 0; // unsupported sampletype

    AUDIO_SAMPLE_TYPE *t_target = target + samplesToMix * 2;
    int sourcepos;

    if (m_buffer->nofChannels() == 2) {         // stereo
        while (target!=t_target) {
            sourcepos = m_fixedPos >> 12;
            target[0] = (((((sampleFunction)
                            (m_buffer, sourcepos, 0) *
                            (4096 - (m_fixedPos & 4095)) +
                            (sampleFunction)(m_buffer, sourcepos + 1, 0) *
                            (m_fixedPos & 4095)) >> 12) *
                          m_fixedLeftVolume) >> 12);

            target[1] = (((((sampleFunction)
                            (m_buffer, sourcepos, 1) *
                            (4096 - (m_fixedPos & 4095)) +
                            (sampleFunction)(m_buffer, sourcepos + 1, 1) *
                            (m_fixedPos & 4095) ) >> 12) *
                          m_fixedRightVolume) >> 12);

            m_fixedPos += m_fixedInc;
            target += 2;
        };
    }
    else {                                      // mono
        int temp;
        while (target != t_target) {
            sourcepos = m_fixedPos >> 12;
            temp = (((sampleFunction)(m_buffer, sourcepos, 0 ) *
                     (4096 - (m_fixedPos & 4095)) +
                     (sampleFunction)(m_buffer, sourcepos + 1, 0) *
                     (m_fixedPos & 4095)) >> 12);

            target[0] = ((temp * m_fixedLeftVolume) >> 12);
            target[1] = ((temp * m_fixedRightVolume) >> 12);
            m_fixedPos += m_fixedInc;
            target += 2;
        }
    }

    return samplesToMix;
}


int AudioBufferPlayInstance::pullAudio(AUDIO_SAMPLE_TYPE *target,
                                       int bufferLength )
{

    if (!m_buffer)
        return 0; // no sample associated to mix..

    int channelLength = ((m_buffer->dataLength()) /
                         (m_buffer->nofChannels() *
                          m_buffer->bytesPerSample())) - 2;

    int samplesToWrite = bufferLength / 2;
    int amount;
    int totalMixed = 0;


    while (samplesToWrite > 0) {
        int samplesLeft = channelLength - (m_fixedPos >> 12);

        // This is how much we can mix at least
        int maxMixAmount = (int)(((long long int)(samplesLeft) << 12) /
                                 m_fixedInc);

        if (maxMixAmount > samplesToWrite) {
            maxMixAmount = samplesToWrite;
        }

        if (maxMixAmount > 0) {
            amount = mixBlock(target+totalMixed * 2, maxMixAmount);
            if (amount == 0) {
                break; // an error occured
            }
            totalMixed += amount;
        }
        else {
            amount = 0;
            m_fixedPos = channelLength<<12;
        }

        // sample is ended,.. check the looping variables and see what to do.
        if ((m_fixedPos >> 12) >= channelLength) {
            m_fixedPos -= (channelLength << 12);
            if (m_loopTimes > 0)
                m_loopTimes--;
            if (m_loopTimes == 0) {
                stop();
                return totalMixed;
            }
        }

        samplesToWrite -= amount;
        if (samplesToWrite < 1)
            break;
    }

    return totalMixed * 2;
}
