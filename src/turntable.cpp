/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

#include <QtGui>
#include <math.h>
#include "pullaudioout.h"
#include "pushaudioout.h"
#include "turntable.h"

using namespace GE;

const int MaxDiskSpeed = 5.0f;

Turntable::Turntable(QSettings *settings, QObject *parent)
    : GE::AudioSource(parent),
      m_defaultSample(":/sounds/ivory.wav"),
      m_defaultVolume(0.65f),
      m_maxLoops(1),
      m_Settings(settings),
      m_buffer(NULL),
      m_decoder(NULL)
{
    m_loops = 0;
    m_pos = 0;
    m_speed = 0.0f;
    m_targetSpeed = 0.0f;
    m_channelLength = 0;
    m_cc = 0;
    m_headOn = false;

    m_audioMixer = new AudioMixer;
    m_audioMixer->addAudioSource(this);
    m_cutOffEffect = new CutOffEffect(this);
    m_cutOffEffect->setCutOff(1.0f);
    m_cutOffEffect->setResonance(1.0f);

#ifdef Q_WS_MAEMO_6
    // Works better with N9
    m_audioOut = new PushAudioOut(m_audioMixer, this);
#else
    m_audioOut = new PullAudioOut(m_audioMixer, this);
#endif

    m_audioMixer->setGeneralVolume(m_Settings->value("Volume",
                                                     m_defaultVolume).toFloat());

#if defined(Q_OS_SYMBIAN) && !defined(Q_OS_SYMBIAN_1)
    m_volumeKeys = new VolumeKeys(this, this);
#endif
}


Turntable::~Turntable()
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
    delete m_volumeKeys;
#endif
}


int Turntable::pullAudio(AUDIO_SAMPLE_TYPE *target, int bufferLength)
{
    QMutexLocker locker(&m_PosMutex);

    if (m_headOn == false || (!m_buffer && !m_decoder)) {
        return 0;
    }

    AUDIO_SAMPLE_TYPE *dst = target;
    AUDIO_SAMPLE_TYPE *t_target = dst + bufferLength;
    SAMPLE_FUNCTION_TYPE sfunc = NULL;

    float speedmul;
    if (m_decoder) {
        speedmul = (float)m_decoder->fileInfo()->sample_rate /
                (float)AUDIO_FREQUENCY * 2048.0f;
    } else {
        sfunc = m_buffer->getSampleFunction();
        if (sfunc == NULL) {
            return 0;
        }
        speedmul = (float)m_buffer->getSamplesPerSec() /
                (float)AUDIO_FREQUENCY * 2048.0f;
    }

    int p;
    int inc = (int)(m_speed * speedmul);
    while (dst != t_target) {
        if (++m_cc > 64) {
            m_speed += (m_targetSpeed - m_speed) * 0.1f;
            if (m_speed < -MaxDiskSpeed)
                m_speed = -MaxDiskSpeed;
            else if (m_speed > MaxDiskSpeed)
                m_speed = MaxDiskSpeed;
            inc = (int)(m_speed * speedmul);
            m_cc = 0;
        }

        if (m_loops >= m_maxLoops && m_pos >= m_channelLength) {
            m_pos = m_channelLength - 1;
        }
        else if (m_pos >= m_channelLength) {
            m_pos %= m_channelLength;
            m_loops++;
            if (m_loops >= m_maxLoops) {
                m_loops = 0;
            }
        }

        if (m_loops == 0 && m_pos < 0) {
            m_pos = 0;
        }
        else if (m_pos < 0) {
            m_pos = m_channelLength - 1 - ((-m_pos) % m_channelLength);
            if (m_loops > 0) {
                m_loops--;
            }
        }

        p = (m_pos >> 11);

        if (m_decoder) {
            *dst++ = m_decoder->at(p << 1);
            *dst++ = m_decoder->at((p << 1) + 1);
        } else {
            *dst++ = (((sfunc)(m_buffer, p, 0) * (2047^(m_pos & 2047)) +
                      (sfunc)(m_buffer, p+1, 0) * (m_pos & 2047)) >> 11);
            *dst++ = (((sfunc)(m_buffer, p, 1) * (2047 ^ (m_pos & 2047)) +
                      (sfunc)(m_buffer, p+1, 1) * (m_pos & 2047)) >> 11);
        }
        m_pos += inc;
    }

    m_cutOffEffect->process(target, bufferLength);

    // Emit signal about the audio position to advance the needle to
    // correct position
    emit audioPosition(1.0 / m_maxLoops * (m_pos * 1.0f / m_channelLength
                                           + m_loops));

    return bufferLength;
}


void Turntable::addAudioSource(GE::AudioSource *source)
{
    m_audioMixer->addAudioSource(source);
}


void Turntable::openSample(const QString &filePath)
{
    QMutexLocker locker(&m_PosMutex);

    QString parsedFilePath;
    if (filePath.isEmpty()) {
        parsedFilePath = m_defaultSample;
    }
    else {
        parsedFilePath = filePath;
#ifdef Q_WS_MAEMO_6
        parsedFilePath.replace(QString("file://"), QString(""));
#else
        parsedFilePath.replace(QString("file:///"), QString(""));
#endif
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
        // Support only stereo oggs for now
        if (m_decoder->fileInfo()->channels != 2) {
            delete m_decoder;
            m_decoder = NULL;
        }
    } else {
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
        if (m_decoder) {
            m_channelLength = (m_decoder->decodedLength()) - 2;
            m_channelLength <<= 11;
        } else {
            m_channelLength = ((m_buffer->getDataLength()) /
                               (m_buffer->getNofChannels() *
                                m_buffer->getBytesPerSample())) - 2;
            m_channelLength <<= 11;
        }
        m_audioMixer->addAudioSource(this);
    }

    // Save the succesfully loaded sample to the persistent storage
    m_Settings->setValue("LastSample", parsedFilePath);

    emit sampleOpened(parsedFilePath);
}


void Turntable::openLastSample()
{
    openSample(m_Settings->value("LastSample", m_defaultSample).toString());
}


void Turntable::setSample(QVariant value)
{
    openSample(value.toString());
}


void Turntable::openDefaultSample()
{
    openSample();
}


void Turntable::setDiscAimSpeed(QVariant value)
{
    float speed = value.toFloat();
    if (speed > -MaxDiskSpeed && speed < MaxDiskSpeed) {
        m_targetSpeed = m_targetSpeed * (1.0f - 0.50f) + speed * 0.50f;
    }
}


void Turntable::setDiscSpeed(QVariant value)
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


void Turntable::setCutOff(QVariant value)
{
    m_cutOffEffect->setCutOff(value.toFloat());
}


void Turntable::setResonance(QVariant value)
{
    m_cutOffEffect->setResonance(powf(value.toFloat(), 2.0f));
}


void Turntable::volumeUp()
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


void Turntable::volumeDown()
{
    float volume = m_audioMixer->generalVolume() * 0.75f;
    if (volume < 0.01f) {
        volume = 0.0f;
    }

    m_audioMixer->setGeneralVolume(volume);
    m_Settings->setValue("Volume", volume);
}

/*!
  The parameter \a position should be in range 0 - 1.0
*/
void Turntable::seekToPosition(QVariant position)
{
    QMutexLocker locker(&m_PosMutex);

    if (!m_buffer && !m_decoder) {
        return;
    }

    float value = position.toFloat();
    m_loops = value / (1.0 / m_maxLoops);
    if (m_loops >= m_maxLoops) {
        m_loops = 0;
    }

    m_pos = (value / (1.0 / m_maxLoops) - m_loops) * m_channelLength;
}

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)

/*!

*/
void Turntable::profile(QSystemDeviceInfo::Profile profile)
{
    if (profile == QSystemDeviceInfo::SilentProfile) {
        m_audioMixer->setGeneralVolume(0.0f);
    }

#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    // In Maemo where there is no way to get volume
    // back if it is set to 0, we set the volume
    // to the default volume when getting out of
    // silent profile. In Maemo the devices volume
    // buttons control the devices volume, in Symbian
    // the devices volume buttons control application
    // specific volume.
    else {
        m_audioMixer->setGeneralVolume(m_defaultVolume);
    }
#endif
}
#endif
