#include <math.h>
#include <QSettings>
#include <QDesktopServices>
#include <QUrl>
#include "TurnTable.h"

#ifdef Q_OS_SYMBIAN
    #include <remconcoreapitarget.h>
    #include <remconinterfaceselector.h>
#endif

using namespace GE;


TurnTable::TurnTable(QSettings *settings)
    : m_Settings(settings)
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

    m_source = CAudioBuffer::loadWav(QString(":/sounds/ivory.wav"));
    m_audioMixer = new CAudioMixer;
    m_audioMixer->addAudioSource(this);
    m_audioOut = new GE::AudioOut(this, m_audioMixer);

    m_audioMixer->setGeneralVolume(m_Settings->value("Volume", 0.65f).toFloat());


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
    AUDIO_SAMPLE_TYPE *t_target = target + bufferLength;
    SAMPLE_FUNCTION_TYPE sfunc = m_source->getSampleFunction();

    int channelLength = ((m_source->getDataLength()) / (m_source->getNofChannels() * m_source->getBytesPerSample())) - 2;
    channelLength<<=11;

    int p;
    int fixedReso = (m_resonanceValue * 4096.0f);
    int fixedCutoff = (m_cutOffValue * 4096.0f);

    float speedmul = (float)m_source->getSamplesPerSec() / (float)AUDIO_FREQUENCY * 2048.0f;
    int inc = (int)(m_speed * speedmul);

    if (m_headOn == false) {
        m_pos = bufferLength / 2 * inc;
        return 0;
    }

    const int maxloops = 5;

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

        if(m_loops >= maxloops && m_pos >= channelLength) {
            m_pos = channelLength - 1;
        }
        else if(m_pos >= channelLength) {
            m_pos %= channelLength;
            m_loops++;
            if(m_loops >= maxloops) {
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

        input = (((sfunc)(m_source, p, 0) * (2047^(m_pos & 2047)) + (sfunc)(m_source, p+1, 0) * (m_pos & 2047)) >> 11);
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

        input = (((sfunc)(m_source, p, 1) * (2047 ^ (m_pos & 2047)) + (sfunc)(m_source, p+1, 1) * (m_pos & 2047)) >> 11);
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

    // Emit signal about the audio position to draw advance the needle to correct position
    emit audioPosition(1.0 / maxloops * (m_pos * 1.0f / channelLength + m_loops));

    return bufferLength;
}


void TurnTable::addAudioSource(GE::IAudioSource *source)
{
    m_audioMixer->addAudioSource(source);
}


void TurnTable::setDiscAimSpeed(QVariant value)
{
    float speed = value.toFloat();
    if(speed > -100.0f && speed < 100.0f) {
        m_targetSpeed = m_targetSpeed * (1.0f - 0.05f) + speed * 0.05f;
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


void TurnTable::linkActivated(QVariant link)
{
    QDesktopServices::openUrl(QUrl(link.toString(), QUrl::TolerantMode));
}
