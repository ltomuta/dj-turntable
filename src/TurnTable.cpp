#include <QSettings>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include <math.h>
#include "TurnTable.h"

#ifdef Q_OS_SYMBIAN
    #include <remconcoreapitarget.h>
    #include <remconinterfaceselector.h>
#endif

using namespace GE;


TurnTable::TurnTable(QSettings *settings)
    : m_defaultSample(":/sounds/ivory.wav"),
      m_defaultVolume(0.65f),
      m_maxLoops(1),
      m_Settings(settings)
{
    m_loops = 0;
    m_pos = 0;
    m_speed = 0.0f;
    m_targetSpeed = 0.0f;
    m_cc = 0;
    m_headOn = false;

    memset(m_lp, 0, sizeof(int) * 2);
    memset(m_bp, 0, sizeof(int) * 2);
    memset(m_hp, 0, sizeof(int) * 2);

    setResonance(1.0f);
    setCutOff(1.0f);

    m_cutOffValue = m_cutOffTarget;
    m_resonanceValue = m_resonanceTarget;

    m_audioMixer = new CAudioMixer;
    m_audioMixer->addAudioSource(this);
    m_audioOut = new GE::AudioOut(this, m_audioMixer);

    m_audioMixer->setGeneralVolume(m_Settings->value("Volume",
                                                     m_defaultVolume).toFloat());


#ifdef Q_OS_SYMBIAN
    Observer *m_Observer = new Observer(this);
    m_Selector = CRemConInterfaceSelector::NewL();
    m_Target = CRemConCoreApiTarget::NewL(*m_Selector, *m_Observer);
    m_Selector->OpenTargetL();
#endif
}


TurnTable::~TurnTable()
{
#ifdef Q_OS_SYMBIAN
    delete m_Target;
    delete m_Selector;
    delete m_Observer;
#endif
}


int TurnTable::pullAudio(AUDIO_SAMPLE_TYPE *target, int bufferLength)
{
    QMutexLocker locker(&m_PosMutex);

    if(m_headOn == false || m_buffer.isNull()) {
        return 0;
    }


    AUDIO_SAMPLE_TYPE *t_target = target + bufferLength;
    SAMPLE_FUNCTION_TYPE sfunc = m_buffer->getSampleFunction();
    if(sfunc == NULL) {
        return 0;
    }

    int channelLength = ((m_buffer->getDataLength()) /
                         (m_buffer->getNofChannels() *
                          m_buffer->getBytesPerSample())) - 2;
    channelLength<<=11;

    int p;
    int fixedReso = (m_resonanceValue * 4096.0f);
    int fixedCutoff = (m_cutOffValue * 4096.0f);

    float speedmul = (float)m_buffer->getSamplesPerSec() /
                     (float)AUDIO_FREQUENCY * 2048.0f;
    int inc = (int)(m_speed * speedmul);

    int input;
    while(target != t_target) {
        if(m_cc > 64) {
            m_speed += (m_targetSpeed - m_speed) * 0.1f;
            m_cutOffValue += (m_cutOffTarget - m_cutOffValue) * 0.1f;
            m_resonanceValue += (m_resonanceTarget - m_resonanceValue) * 0.1f;
            inc = (int)(m_speed * speedmul);
            fixedReso = (m_resonanceValue * 4096.0f);
            fixedCutoff = (m_cutOffValue * 4096.0f);
            m_cc = 0;
        }
        else {
            m_cc++;
        }

        if(m_loops >= m_maxLoops && m_pos >= channelLength) {
            m_pos = channelLength - 1;
        }
        else if(m_pos >= channelLength) {
            m_pos %= channelLength;
            m_loops++;
            if(m_loops >= m_maxLoops) {
                m_loops = 0;
            }
        }

        if(m_loops == 0 && m_pos < 0) {
            m_pos = 0;
        }
        else if(m_pos < 0) {
            m_pos = channelLength - 1 - ((-m_pos) % channelLength);
            if(m_loops > 0) {
                m_loops--;
            }
        }

        p = (m_pos >> 11);

        input = (((sfunc)(m_buffer, p, 0) * (2047^(m_pos & 2047)) +
                  (sfunc)(m_buffer, p+1, 0) * (m_pos & 2047)) >> 11);
        m_lp[0] += ((m_bp[0] * fixedCutoff) >> 12);
        m_hp[0] = input - m_lp[0] - ((m_bp[0] * fixedReso) >> 12);
        m_bp[0] += ((m_hp[0] * fixedCutoff) >> 12);

        input = m_lp[0];
        if(input < -32767) {
            input = -32767;
        }
        if(input > 32767) {
            input = 32767;
        }

        target[0] = input;

        input = (((sfunc)(m_buffer, p, 1) * (2047 ^ (m_pos & 2047)) +
                  (sfunc)(m_buffer, p+1, 1) * (m_pos & 2047)) >> 11);
        m_lp[1] += ((m_bp[1] * fixedCutoff) >> 12);
        m_hp[1] = input - m_lp[1] - ((m_bp[1] * fixedReso) >> 12);
        m_bp[1] += ((m_hp[1] * fixedCutoff) >> 12);

        input = m_lp[1];
        if(input < -32767) {
            input = -32767;
        }
        if(input > 32767) {
            input = 32767;
        }

        target[1] = input;
        target += 2;
        m_pos += inc;
    }

    // Emit signal about the audio position to advance the needle to
    // correct position
    emit audioPosition(1.0 / m_maxLoops * (m_pos * 1.0f / channelLength
                                           + m_loops));

    return bufferLength;
}


void TurnTable::addAudioSource(GE::IAudioSource *source)
{
    m_audioMixer->addAudioSource(source);
}


void TurnTable::openSample(const QString &filePath)
{
    QMutexLocker locker(&m_PosMutex);

    QString parsedFilePath;
    if(filePath.isEmpty()) {
        parsedFilePath = m_defaultSample;
    }
    else {
        parsedFilePath = filePath;
        parsedFilePath.replace(QString("file:///"), QString(""));
    }

    if(m_audioMixer->removeAudioSource(this) == false) {
        return;
    }

    delete m_buffer;
    m_pos = 0;
    m_loops = 0;

    m_buffer = CAudioBuffer::loadWav(parsedFilePath);

    if(m_buffer.isNull()) {
        // Failed to load sample
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
    if(speed > -100.0f && speed < 100.0f) {
        m_targetSpeed = m_targetSpeed * (1.0f - 0.10f) + speed * 0.10f;
    }
}


void TurnTable::setDiscSpeed(QVariant value)
{
    float speed = value.toFloat();
    if(speed > -100.0f && speed < 100.0f) {
        m_targetSpeed = speed;
    }
}


void TurnTable::setCutOff(QVariant value)
{
    m_cutOffTarget = value.toFloat();
}


void TurnTable::setResonance(QVariant value)
{
    m_resonanceTarget = powf(value.toFloat(), 2.0f);
}


void TurnTable::volumeUp()
{
    float volume = m_audioMixer->getGeneralVolume() * 1.333f;
    if(volume == 0.0f) {
        volume = 0.01;
    }
    else if(volume >= 0.95f) {
        volume = 0.95f;
    }

    m_audioMixer->setGeneralVolume(volume);
    m_Settings->setValue("Volume", volume);
}


void TurnTable::volumeDown()
{
    float volume = m_audioMixer->getGeneralVolume() * 0.75f;
    if(volume < 0.01f) {
        volume = 0.0f;
    }

    m_audioMixer->setGeneralVolume(volume);
    m_Settings->setValue("Volume", volume);
}


// The parameter position should be in range 0 - 1.0
void TurnTable::seekToPosition(QVariant position)
{
    QMutexLocker locker(&m_PosMutex);

    int channelLength = ((m_buffer->getDataLength()) /
                         (m_buffer->getNofChannels() *
                          m_buffer->getBytesPerSample())) - 2;
    channelLength <<= 11;

    float value = position.toFloat();
    m_loops = value / (1.0 / m_maxLoops);
    if(m_loops >= m_maxLoops) {
        m_loops = 0;
    }

    m_pos = (value / (1.0 / m_maxLoops) - m_loops) * channelLength;
}


void TurnTable::linkActivated(QVariant link)
{
    QDesktopServices::openUrl(QUrl(link.toString(), QUrl::TolerantMode));
}
