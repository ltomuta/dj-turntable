#include "TurnTable.h"

using namespace GE;


CScratchDisc::CScratchDisc( GE::CAudioBuffer *discSource )
{
    m_source = discSource;
    m_pos = 0;
    m_speed = 0.0f;
    m_targetSpeed = 0.0f;
    m_cc = 0;
    m_headOn = false;
}


CScratchDisc::~CScratchDisc()
{
}


void CScratchDisc::setSpeed(float speed)
{
    if(speed > -100.0f && speed < 100.0f) {
        m_targetSpeed = speed;
    }
}


int CScratchDisc::pullAudio( AUDIO_SAMPLE_TYPE *target, int bufferLength )
{
    if(m_source == NULL) {
        return 0;
    }

    AUDIO_SAMPLE_TYPE *t_target = target+bufferLength;
    SAMPLE_FUNCTION_TYPE sfunc = m_source->getSampleFunction();
    int channelLength = ((m_source->getDataLength()) / (m_source->getNofChannels() * m_source->getBytesPerSample()))-1;
    channelLength <<= 12;
    int p;

    float speedmul = (float)m_source->getSamplesPerSec() / (float)AUDIO_FREQUENCY * 4096.0f;
    int inc = (int)(m_speed*speedmul);

    if (m_headOn == false) {
        m_targetSpeed = m_speed;
        m_pos = bufferLength / 2 * inc;
        return 0;
    }

    while (target!=t_target) {
        if (m_cc>128) {
            m_speed += (m_targetSpeed - m_speed)*0.1f;
            inc = (int)(m_speed*speedmul);
            m_cc = 0;
        } else m_cc++;
        if (m_pos>=channelLength) m_pos %= channelLength;
        if (m_pos<0) m_pos = channelLength-1-((-m_pos)%channelLength);
        p = (m_pos>>12);
        target[0] = (((sfunc)( m_source, p, 0 ) * (4095^(m_pos&4095)) + (sfunc)( m_source, p+1, 0 )*(m_pos&4095))>>12);
        target[1] = (((sfunc)( m_source, p, 1 ) * (4095^(m_pos&4095)) + (sfunc)( m_source, p+1, 1 )*(m_pos&4095))>>12);

        target+=2;
        m_pos += inc;
    };

    return bufferLength;
}


TurnTable::TurnTable()
{
    m_discSample = GE::CAudioBuffer::loadWav(QString(":/sounds/melody.wav"));

    m_sdisc = new CScratchDisc(m_discSample);
    m_audioMixer.addAudioSource(m_sdisc);
    m_audioOut = new GE::AudioOut(this, &m_audioMixer);

    m_audioMixer.setGeneralVolume(0.4999f);
    setDiscSpeed(1.0f);
}


void TurnTable::addAudioSource(GE::IAudioSource *source)
{
    m_audioMixer.addAudioSource(source);
}


void TurnTable::setDiscSpeed(QVariant speed)
{
    m_sdisc->setSpeed(speed.toFloat());
}


TurnTable::~TurnTable()
{
    if (m_audioOut) {
        delete m_audioOut;
        m_audioOut = NULL;
    }

    m_audioOut = NULL;
    m_discSample = NULL;
}
