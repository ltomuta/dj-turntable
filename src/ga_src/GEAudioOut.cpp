#include <QtCore/qstring.h>
#include <QAudioOutput>

#include "GEAudioOut.h"

#ifdef Q_OS_SYMBIAN
    #include <SoundDevice.h>
#endif

using namespace GE;


const int CHANNELS = 2;
const QString CODEC = "audio/pcm";
const QAudioFormat::Endian BYTEORDER = QAudioFormat::LittleEndian;
const QAudioFormat::SampleType SAMTYPE = QAudioFormat::SignedInt;



AudioOut::AudioOut(QObject *parent, GE::IAudioSource *source) :
        QThread(parent) {
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

    m_audioOutput = new QAudioOutput(info, format);

#ifdef Q_WS_MAEMO_5
    m_audioOutput->setBufferSize(20000);
    m_sendBufferSize = 5000;
#else
    m_audioOutput->setBufferSize(16000);
    m_sendBufferSize = 4000;
#endif

    m_outTarget = m_audioOutput->start();

    m_sendBuffer = new AUDIO_SAMPLE_TYPE[m_sendBufferSize];
    m_samplesMixed = 0;

    m_runstate = 0;

#ifndef Q_OS_SYMBIAN
    start();
#else
    m_audioOutput->setNotifyInterval(5);
    connect(m_audioOutput, SIGNAL(notify()), this, SLOT(audioNotify()));

    // Really ugly hack is used as a last resort. This allows us to adjust the
    // application volume in Symbian. The CMMFDevSound object lies deep
    // inside the QAudioOutput in Symbian implementation and it has the needed
    // functions. So we get the needed object accessing directly from memory.
    unsigned int *pointer_to_abstract_audio =
            (unsigned int*)((unsigned char*)m_audioOutput + 8);

    unsigned int *dev_sound_wrapper =
            (unsigned int*)(*pointer_to_abstract_audio) + 13;

    unsigned int *temp = ((unsigned int*)(*dev_sound_wrapper) + 6);

    CMMFDevSound *dev_sound = (CMMFDevSound*)(*temp);
    dev_sound->SetVolume(dev_sound->MaxVolume());
#endif
}


AudioOut::~AudioOut() {
    if (m_runstate == 0)
        m_runstate = 1;

    if (QThread::isRunning() == false)
        m_runstate = 2;

    while (m_runstate != 2) {
        // wait until the thread is finished
        msleep(50);
    }

    m_audioOutput->stop();

    delete m_audioOutput;
    delete [] m_sendBuffer;
}


void AudioOut::audioNotify() {
    tick();
}

void AudioOut::tick() {
    // fill data to buffer as much as free space is available..
    int samplesToWrite = m_audioOutput->bytesFree() /
                         (CHANNELS*AUDIO_SAMPLE_BITS/8);

    samplesToWrite *= 2;

    if (samplesToWrite > m_sendBufferSize)
        samplesToWrite = m_sendBufferSize;

    if (samplesToWrite <= 0)
        return;

    int mixedSamples = m_source->pullAudio(m_sendBuffer, samplesToWrite);
    m_outTarget->write((char*)m_sendBuffer, mixedSamples * 2);
}


void AudioOut::run() {
    if (!m_source) {
        m_runstate = 2;
        return;
    }

    int sleepTime = m_sendBufferSize * 340 / AUDIO_FREQUENCY;
    if (sleepTime < 2)
        sleepTime = 2;

    while (m_runstate == 0) {
        tick();
        msleep(sleepTime);
    }

    m_runstate = 2;
}
