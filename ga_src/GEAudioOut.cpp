/**
 *
 * GE::GA Qt Audio out
 * tuomo.hirvonen@digia.com
 *
 */

#include <QtCore/qstring.h>
//#include <QtMultimedia/qaudiooutput.h>
//#include <QtMultimedia/qaudioformat.h>
#include <QAudioOutput>

#include "GEAudioOut.h"

using namespace GE;
//using namespace QTM_NAMESPACE;

/*
#ifndef Q_OS_WIN32
QTM_USE_NAMESPACE
#endif
*/

const int CHANNELS = 2;
const QString CODEC = "audio/pcm";
const QAudioFormat::Endian BYTEORDER = QAudioFormat::LittleEndian;
const QAudioFormat::SampleType SAMTYPE = QAudioFormat::SignedInt;



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





            // debug, check the error and the state
    //QAudio::Error err = m_audioOutput->error();
    //QAudio::State state = m_audioOutput->state();

    m_sendBufferSize = 2000;
    m_sendBuffer = new AUDIO_SAMPLE_TYPE[ m_sendBufferSize ];


    m_audioOutput = new QAudioOutput(info,format);
    m_audioOutput->setBufferSize(6000);
    connect(m_audioOutput,SIGNAL(notify()),SLOT(audioNotify()));
    m_audioOutput->setNotifyInterval(5);
    m_outTarget = m_audioOutput->start();


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


