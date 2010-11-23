#include "TurnTable.h"

using namespace GE;

const unsigned char drum_seq0[] = {5,0,1,0,9,0,5,0, 5,0,1,0,9,4,2,0, 5,0,1,0,9,0,1,4, 1,4,1,0,9,0,2,0 };

const unsigned char drum_seq1[] = {21,0,1,0,14,0,0,0, 5,0,1,0,14,0,0,8, 5,0,1,0,14,0,0,0, 5,2,0,0,6,0,1,8,
                                   21,0,1,0,14,0,0,0, 5,0,1,0,14,0,0,8, 5,0,1,0,14,0,0,0, 5,2,0,0,14,12,13,13 };

const unsigned char drum_seq2[] = {5,0,1,0,33,0,5,0, 5,0,1,0,33,0,2,0 };

const unsigned char drum_seq3[] = {5,0,1,0,10,4,1,0, 5,0,1,0,10,4,5,4 };



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


void CScratchDisc::setSpeed( float speed ) {
    if (speed<-100.0f || speed > 100.0f){
        return;
    }

    m_targetSpeed = speed;
}


int CScratchDisc::pullAudio( AUDIO_SAMPLE_TYPE *target, int bufferLength )
{
    if (!m_source) {
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


CTurnTable::CTurnTable()
{
    m_discSample = GE::CAudioBuffer::loadWav(QString(":/sounds/melody.wav"));

    m_sdisc = new CScratchDisc(m_discSample);
    m_audioMixer.addAudioSource(m_sdisc);
    m_audioOut = new GE::AudioOut(this, &m_audioMixer);

    m_drumMachine = new CDrumMachine();
    m_drumMachine->setBpm( 600 );
    m_audioMixer.addAudioSource( m_drumMachine );

    m_audioMixer.setGeneralVolume(0.4999f);
    setDiscSpeed(1.0f);
}


void CTurnTable::setDiscSpeed(QVariant speed)
{
    m_sdisc->setSpeed(speed.toFloat());
}


CTurnTable::~CTurnTable()
{
    if (m_audioOut) {
        delete m_audioOut;
        m_audioOut = NULL;
    }

    m_audioOut = NULL;
    m_discSample = NULL;
    m_drumMachine = NULL;
}


void CTurnTable::toggleBeat(QVariant index)
{
    switch (index.toInt() ) {
    default:
        m_drumMachine->setSeq(0,0);
        break;
    case 0:
        m_drumMachine->setSeq(drum_seq0, 64);
        break;
    case 1:
        m_drumMachine->setSeq(drum_seq1, 16);
        break;
    case 2:
        m_drumMachine->setSeq(drum_seq2, 16);
        break;
    case 3:
        m_drumMachine->setSeq(drum_seq3, 32);
        break;
    };

    emit drumButtons(m_drumMachine->getSeqLen(), m_drumMachine->getSampleCount());


};



