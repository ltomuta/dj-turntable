/**
 *
 * GE::GA Qt Audio out
 * tuomo.hirvonen@digia.com
 *
 */

#include <QtCore/qstring.h>
#include <QAudioOutput>
#include <QDebug>
#include "GEAudioOut.h"

#ifdef Q_OS_SYMBIAN
    #include <SoundDevice.h>
#endif

using namespace GE;


const int CHANNELS = 2;
const QString CODEC = "audio/pcm";
const QAudioFormat::Endian BYTEORDER = QAudioFormat::LittleEndian;
const QAudioFormat::SampleType SAMTYPE = QAudioFormat::SignedInt;

/*
void debugDumpMemory( void *start, int bytesToDump, const char *message ) {
    char testr[128];
    char line[256];
    qDebug() << "-----------------------------------------------------";
    sprintf(testr, "(%s): Dumping %d bytes from %x", message, bytesToDump, start );
    qDebug() << testr;
    line[0] = 0;
    int l1=0;
    int i1=0;
    unsigned char *p = (unsigned char*)start;
    for (int f=0; f<bytesToDump; f++) {
        sprintf(testr, "%2x", p[f] );
        strcat(line, testr );

        i1++;
        if (i1 > 3 && f<bytesToDump-1) { strcat(line, " - "); i1=0; l1++; }
        else if (f<bytesToDump-1) strcat( line, ":");

        if (l1>3) {
            qDebug() << line;
            line[0] = 0;
            l1=0;
        }


    };
    if (line[0]!=0) qDebug() << line;
    qDebug() << "-----------------------------------------------------";
}
*/

AudioOut::AudioOut( QObject *parent, GE::IAudioSource *source ) : QThread(parent) {         // qobject
    m_source = source;
    QAudioFormat format;
    format.setFrequency(AUDIO_FREQUENCY);
    format.setChannels(CHANNELS);
    format.setSampleSize(AUDIO_SAMPLE_BITS);
    format.setCodec(CODEC);
    format.setByteOrder(BYTEORDER);
    format.setSampleType(SAMTYPE);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format))
        format = info.nearestFormat(format);

    m_sendBufferSize = 3000;
    m_sendBuffer = new AUDIO_SAMPLE_TYPE[ m_sendBufferSize ];

    m_audioOutput = new QAudioOutput(info,format);
    m_audioOutput->setBufferSize(6000);
    connect(m_audioOutput,SIGNAL(notify()),SLOT(audioNotify()));
    m_audioOutput->setNotifyInterval(5);
    m_outTarget = m_audioOutput->start();


#ifdef Q_OS_SYMBIAN

    //qDebug() << "sizeof qobject:" << sizeof(QObject);
    unsigned int *pointer_to_abstract_audio = (unsigned int*)( (unsigned char*)m_audioOutput + 8 );

    //debugDumpMemory(m_audioOutput, sizeof(QAudioOutput), "QAudioOut");
    //qDebug() << "Error:"<<m_audioOutput->error()<<" State:" << m_audioOutput->state();
    //debugDumpMemory((unsigned int*)(*pointer_to_abstract_audio), 128, "QAbstractAudioOutput");

    //qDebug() << "QAudio::State size: " << sizeof(QAudio::State);

    unsigned int *dev_sound_wrapper = (unsigned int*)(*pointer_to_abstract_audio) + 13;
    //debugDumpMemory((unsigned int*)(*dev_sound_wrapper), 32, "dev_sound_wrapper");

    unsigned int *temp = ((unsigned int*)(*dev_sound_wrapper) + 6);
    CMMFDevSound *dev_sound = (CMMFDevSound*)(*temp); //(CMMFDevSound*)((unsigned char*)(*dev_sound_wrapper) + 6 * 4);

    //debugDumpMemory(dev_sound, sizeof(CMMFDevSound), "dev_sound");

    dev_sound->SetVolume(dev_sound->MaxVolume());
#endif


    m_samplesMixed = 0;
    m_runstate=0;
};


AudioOut::~AudioOut() {
    if (m_runstate==0) m_runstate = 1;
    if (QThread::isRunning() == false) m_runstate = 2;
    while (m_runstate!=2) { msleep(50); }       // wait until the thread is finished
    m_audioOutput->stop();
    delete m_audioOutput;
    delete [] m_sendBuffer;
};


void AudioOut::audioNotify() {
    tick();
};

void AudioOut::tick() {
    // fill data to buffer as much as free space is available..
    int samplesToWrite = m_audioOutput->bytesFree() / (CHANNELS*AUDIO_SAMPLE_BITS/8);
    samplesToWrite*=2;

    if (samplesToWrite > m_sendBufferSize) samplesToWrite = m_sendBufferSize;
    if (samplesToWrite<=0) return;
    int mixedSamples = m_source->pullAudio( m_sendBuffer, samplesToWrite );
    m_outTarget->write( (char*)m_sendBuffer, mixedSamples*2 );


    /*
    qint64 processedUsecs = m_audioOutput->processedUSecs();
    //float secsMixed = (float)processed / 1000000.0f;

    qint64 bytesInBuffer = m_audioOutput->bufferSize() - m_audioOutput->bytesFree();
    qint64 usInBuffer = (qint64)(1000000) * bytesInBuffer / ( CHANNELS * AUDIO_SAMPLE_BITS / 8 ) / AUDIO_FREQUENCY;
    qint64 processed = (processedUsecs - usInBuffer) * AUDIO_FREQUENCY / 1000000;

    qint64 mixed = (m_samplesMixed>>1);


    //qint64 usPlayed = processed - usInBuffer;

    //int ofs = mixed-samplesProcessed;
    //int writeSize = ((int)( (samplesProcessed + m_sendBufferSize*2) - m_samplesMixed) ) * 2;
    int writeSize = ((processed+m_sendBufferSize)-mixed)*2;


    // if ofs kasvaa liian isoksi,... reset playing position.
    //qint64 ofs = (qint64)(m_samplesMixed>>1)-usPlayed;
    //if (mixed<=samplesProcessed || processedUsecs==0) {         // try to process more
    if (writeSize>0 || processedUsecs==0) {
        //int writeSize = m_sendBufferSize;

        qint64 samplesFree = m_audioOutput->bytesFree() / (AUDIO_SAMPLE_BITS/8);
        if (writeSize>m_sendBufferSize) writeSize = m_sendBufferSize;
        if (writeSize > samplesFree) writeSize = samplesFree;
        int wroteSamples = m_source->pullAudio( m_sendBuffer, writeSize );
        qint64 sentSamples = m_outTarget->write( (char*)m_sendBuffer, wroteSamples*2 ) / 2;
        m_samplesMixed+=sentSamples;//(qint64)wroteSamples;
        if (processedUsecs==0) m_samplesMixed = 0;          // symbianHack

    };
*/
};


void AudioOut::run() {
    if (!m_source) { m_runstate=2; return; }
    int sleepTime = m_sendBufferSize * 400 / AUDIO_FREQUENCY;

    while (m_runstate==0) {
        tick();
        //msleep(sleepTime);
    };
    m_runstate = 2;
};


