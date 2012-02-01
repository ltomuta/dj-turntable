/**
 * Copyright (c) 2011 Nokia Corporation.
 */

#include <QtGui>
#include <math.h>
#include "pullaudioout.h"
#include "TurnTable.h"

#if defined(Q_OS_SYMBIAN) && !defined(Q_OS_SYMBIAN_1)
    #include <remconcoreapitarget.h>
    #include <remconinterfaceselector.h>
#endif

using namespace GE;

const int MaxDiskSpeed = 5.0f;

TurnTable::TurnTable(QSettings *settings, QObject *parent)
    : GE::AudioSource(parent),
      m_defaultSample(":/sounds/ivory.wav"),
      m_defaultVolume(0.15f),
      m_maxLoops(1),
      m_Settings(settings),
      m_buffer(NULL),
      m_decoder(NULL)
{
    m_loops = 0;
    m_pos = 0;
    m_speed = 0.0f;
    m_targetSpeed = 0.0f;
    m_cc = 0;
    m_headOn = false;

    m_audioMixer = new AudioMixer;
    m_audioMixer->addAudioSource(this);
    m_cutOffEffect = new CutOffEffect(this);
    m_cutOffEffect->setCutOff(1.0f);
    m_cutOffEffect->setResonance(1.0f);
    m_audioMixer->setEffect(m_cutOffEffect);
    m_audioOut = new PullAudioOut(m_audioMixer, this);

    m_audioMixer->setGeneralVolume(m_Settings->value("Volume",
                                                     m_defaultVolume).toFloat());

    m_audioMixer->setGeneralVolume(1.0);

#if defined(Q_OS_SYMBIAN) && !defined(Q_OS_SYMBIAN_1)
    Observer *m_Observer = new Observer(this);
    m_Selector = CRemConInterfaceSelector::NewL();
    m_Target = CRemConCoreApiTarget::NewL(*m_Selector, *m_Observer);
    m_Selector->OpenTargetL();
#endif
}


TurnTable::~TurnTable()
{
    m_PosMutex.lock();

    m_audioMixer->removeAudioSource(this);

    delete m_buffer;
    delete m_decoder;

    m_PosMutex.unlock();

    delete m_audioOut;
    delete m_audioMixer;
    delete m_cutOffEffect;

#if defined(Q_OS_SYMBIAN) && !defined(Q_OS_SYMBIAN_1)
    delete m_Target;
#endif
}


int TurnTable::pullAudio(AUDIO_SAMPLE_TYPE *target, int bufferLength)
{
    QMutexLocker locker(&m_PosMutex);

    if (m_headOn == false || (!m_buffer && !m_decoder)) {
        return 0;
    }

    AUDIO_SAMPLE_TYPE *t_target = target + bufferLength;
    SAMPLE_FUNCTION_TYPE sfunc = NULL;

    int64_t channelLength;
    float speedmul;
    if (m_decoder) {
        channelLength = (m_decoder->decodedLength()) - 2;
        channelLength <<= 11;
        speedmul = (float)m_decoder->fileInfo()->sample_rate /
                (float)AUDIO_FREQUENCY * 2048.0f;
    } else {
        sfunc = m_buffer->getSampleFunction();
        if (sfunc == NULL) {
            return 0;
        }
        channelLength = ((m_buffer->getDataLength()) /
                         (m_buffer->getNofChannels() *
                          m_buffer->getBytesPerSample())) - 2;
        channelLength <<= 11;
        speedmul = (float)m_buffer->getSamplesPerSec() /
                (float)AUDIO_FREQUENCY * 2048.0f;
    }

    int p;
    int inc = (int)(m_speed * speedmul);
    while (target != t_target) {
        if (++m_cc > 64) {
            m_speed += (m_targetSpeed - m_speed) * 0.1f;
            if (m_speed < -MaxDiskSpeed)
                m_speed = -MaxDiskSpeed;
            else if (m_speed > MaxDiskSpeed)
                m_speed = MaxDiskSpeed;
            inc = (int)(m_speed * speedmul);
            m_cc = 0;
        }

        if (m_loops >= m_maxLoops && m_pos >= channelLength) {
            m_pos = channelLength - 1;
        }
        else if (m_pos >= channelLength) {
            m_pos %= channelLength;
            m_loops++;
            if (m_loops >= m_maxLoops) {
                m_loops = 0;
            }
        }

        if (m_loops == 0 && m_pos < 0) {
            m_pos = 0;
        }
        else if (m_pos < 0) {
            m_pos = channelLength - 1 - ((-m_pos) % channelLength);
            if (m_loops > 0) {
                m_loops--;
            }
        }

        p = (m_pos >> 11);

        if (m_decoder) {
            target[0] = m_decoder->at(p << 1);
            target[1] = m_decoder->at((p << 1) + 1);
        } else {
            target[0] = (((sfunc)(m_buffer, p, 0) * (2047^(m_pos & 2047)) +
                      (sfunc)(m_buffer, p+1, 0) * (m_pos & 2047)) >> 11);
            target[1] = (((sfunc)(m_buffer, p, 1) * (2047 ^ (m_pos & 2047)) +
                      (sfunc)(m_buffer, p+1, 1) * (m_pos & 2047)) >> 11);
        }
        target += 2;
        m_pos += inc;
    }

    // Emit signal about the audio position to advance the needle to
    // correct position
    emit audioPosition(1.0 / m_maxLoops * (m_pos * 1.0f / channelLength
                                           + m_loops));

    return bufferLength;
}


void TurnTable::addAudioSource(GE::AudioSource *source)
{
    m_audioMixer->addAudioSource(source);
}


void TurnTable::openSample(const QString &filePath)
{
    QMutexLocker locker(&m_PosMutex);

    QString parsedFilePath;
    if (filePath.isEmpty()) {
        parsedFilePath = m_defaultSample;
    }
    else {
        parsedFilePath = filePath;
        parsedFilePath.replace(QString("file:///"), QString(""));
    }

    // Prevents the audio engine to play turntable sound,
    // we are about to delete it
    m_audioMixer->removeAudioSource(this);

    m_pos = 0;
    m_loops = 0;

    delete m_buffer;
    m_buffer = NULL;

    delete m_decoder;
    m_decoder = NULL;

    if (parsedFilePath.endsWith(".ogg", Qt::CaseInsensitive)) {
        // Decode the ogg file on the fly
        m_decoder = new VorbisDecoder(true, this);
        m_decoder->load(parsedFilePath);
    } else {
        delete m_buffer;
        m_buffer = AudioBuffer::load(parsedFilePath);
    }

    if (!m_buffer && !m_decoder) {
        // Failed to load sample
        emit powerOff();
        emit sampleOpened("");
        emit error(parsedFilePath, "");
        return;
    }
    else {
        m_audioMixer->addAudioSource(this);
    }

    // Save the succesfully loaded sample to the persistent storage
    m_Settings->setValue("LastSample", parsedFilePath);

    emit sampleOpened(parsedFilePath);
}


void TurnTable::openLastSample()
{
    openSample(m_Settings->value("LastSample", m_defaultSample).toString());
}


void TurnTable::setSample(QVariant value)
{
    openSample(value.toString());
}


void TurnTable::openDefaultSample()
{
    openSample();
}


void TurnTable::setDiscAimSpeed(QVariant value)
{
    float speed = value.toFloat();
    if (speed > -MaxDiskSpeed && speed < MaxDiskSpeed) {
        m_targetSpeed = m_targetSpeed * (1.0f - 0.50f) + speed * 0.50f;
    }
}


void TurnTable::setDiscSpeed(QVariant value)
{
    float speed = value.toFloat();
    if (speed < -MaxDiskSpeed) {
        m_targetSpeed = -MaxDiskSpeed;
    } else if (speed > MaxDiskSpeed) {
        m_targetSpeed = MaxDiskSpeed;
    } else {
        m_targetSpeed = speed;
    }
}


void TurnTable::setCutOff(QVariant value)
{
    m_cutOffEffect->setCutOff(value.toFloat());
}


void TurnTable::setResonance(QVariant value)
{
    m_cutOffEffect->setResonance(powf(value.toFloat(), 2.0f));
}


void TurnTable::volumeUp()
{
    float volume = m_audioMixer->generalVolume() * 1.333f;
    if (volume == 0.0f) {
        volume = 0.01;
    }
    else if (volume >= 0.95f) {
        volume = 0.95f;
    }

    m_audioMixer->setGeneralVolume(volume);
    m_Settings->setValue("Volume", volume);
}


void TurnTable::volumeDown()
{
    float volume = m_audioMixer->generalVolume() * 0.75f;
    if (volume < 0.01f) {
        volume = 0.0f;
    }

    m_audioMixer->setGeneralVolume(volume);
    m_Settings->setValue("Volume", volume);
}


// The parameter position should be in range 0 - 1.0
void TurnTable::seekToPosition(QVariant position)
{
    QMutexLocker locker(&m_PosMutex);

    if (!m_buffer && !m_decoder) {
        return;
    }

    int64_t channelLength;
    if (m_decoder) {
        channelLength = (m_decoder->decodedLength()) - 2;
    } else {
        channelLength = ((m_buffer->getDataLength()) /
                         (m_buffer->getNofChannels() *
                          m_buffer->getBytesPerSample())) - 2;
    }
    channelLength <<= 11;

    float value = position.toFloat();
    m_loops = value / (1.0 / m_maxLoops);
    if (m_loops >= m_maxLoops) {
        m_loops = 0;
    }

    m_pos = (value / (1.0 / m_maxLoops) - m_loops) * channelLength;
}
