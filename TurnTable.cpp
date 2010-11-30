#include "TurnTable.h"
#include <math.h>

using namespace GE;


CScratchDisc::CScratchDisc( GE::CAudioBuffer *discSource )
{
    m_source = discSource;
    m_pos = 0;
    m_speed = 0.0f;
    m_targetSpeed = 0.0f;
    m_cc = 0;
    m_headOn = false;
    setResonance( 1.0f );
   setCutoff( 1.0f );

   memset( m_lp, 0, sizeof(int)*2 );
   memset( m_bp, 0, sizeof(int)*2 );
   memset( m_hp, 0, sizeof(int)*2 );

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
    int channelLength = ((m_source->getDataLength()) / (m_source->getNofChannels() * m_source->getBytesPerSample()))-2;
    channelLength <<= 12;
    int p;






    int fixedCutoff, fixedReso;

    fixedReso = (m_resonanceValue * 4096.0f );
    fixedCutoff = (m_cutOffValue * 4096.0f );

    float speedmul = (float)m_source->getSamplesPerSec() / (float)AUDIO_FREQUENCY * 4096.0f;
    int inc = (int)(m_speed*speedmul);

    if (m_headOn == false) {
        m_targetSpeed = m_speed;
        m_pos = bufferLength / 2 * inc;
        return 0;
    }

    //static float testAngle = 0.0f;

    int input;
    while (target!=t_target) {
        if (m_cc>128) {
            //testAngle+=0.01f;

            //m_resonanceValue = 0.01f;

            //m_resonanceValue = 1.0f;
            //m_resonanceValue = 0.5f + sinf( testAngle ) *0.45f;
            //m_cutOffValue = 0.5f + cosf( testAngle / 1.4f) *0.45f;

            //fixedReso = (m_resonanceValue * 4096.0f );
            //fixedCutoff = (m_cutOffValue * 4096.0f );
            m_speed += (m_targetSpeed - m_speed)*0.1f;
            inc = (int)(m_speed*speedmul);
            m_cc = 0;
        } else m_cc++;
        if (m_pos>=channelLength) m_pos %= channelLength;
        if (m_pos<0) m_pos = channelLength-1-((-m_pos)%channelLength);
        p = (m_pos>>12);

        //target[0] = (((sfunc)( m_source, p, 0 ) * (4095^(m_pos&4095)) + (sfunc)( m_source, p+1, 0 )*(m_pos&4095))>>12);
        //target[1] = (((sfunc)( m_source, p, 1 ) * (4095^(m_pos&4095)) + (sfunc)( m_source, p+1, 1 )*(m_pos&4095))>>12);


        input = (((sfunc)( m_source, p, 0 ) * (4095^(m_pos&4095)) + (sfunc)( m_source, p+1, 0 )*(m_pos&4095))>>12);
        m_lp[0] += ((m_bp[0]*fixedCutoff)>>12);
        m_hp[0] = input - m_lp[0] - ((m_bp[0] * fixedReso)>>12);
        m_bp[0] += ((m_hp[0]*fixedCutoff)>>12);
        input = m_lp[0];
        if (input<-32767) input = -32767;
        if (input>32767) input = 32767;
        target[0] = input;



        input = (((sfunc)( m_source, p, 1 ) * (4095^(m_pos&4095)) + (sfunc)( m_source, p+1, 1 )*(m_pos&4095))>>12);
        m_lp[1] += ((m_bp[1]*fixedCutoff)>>12);
        m_hp[1] = input - m_lp[1] - ((m_bp[1] * fixedReso)>>12);
        m_bp[1] += ((m_hp[1]*fixedCutoff)>>12);
        input = m_lp[1];
        if (input<-32767) input = -32767;
        if (input>32767) input = 32767;
        target[1] = input;


        /*
    lp += f1*bp;
    hp = input - lp - bp*q;
    bp += f2*hp;
*/

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