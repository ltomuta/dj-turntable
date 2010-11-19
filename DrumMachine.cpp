
#include "DrumMachine.h"

using namespace GE;

const char *drum_sample_files[DRUM_MACHINE_SAMPLE_COUNT] = {
    ":/hihat.wav",
    ":/hihat_open.wav",
    ":/bassd.wav",
    ":/snare.wav",
    ":/cymbal.wav",
    ":/cowbell.wav"
};



CDrumMachine::CDrumMachine() {
    m_mixer = new CAudioMixer();
    for (int f=0; f<DRUM_MACHINE_SAMPLE_COUNT; f++) {
        m_drumSamples[f] =  CAudioBuffer::loadWav( QString( drum_sample_files[f] ) );
        m_playInstances[f] = new CAudioBufferPlayInstance;
        m_playInstances[f]->setDestroyWhenFinished( false );            // dont destroy object when playing is finished>
        m_mixer->addAudioSource( m_playInstances[f] );
    };

    m_seq = 0;
    m_seqLen = 0;
    m_tickCount = 0;
    setBpm( 440 );
};

CDrumMachine::~CDrumMachine() {
    delete m_mixer;
    for (int f=0; f<DRUM_MACHINE_SAMPLE_COUNT; f++) {
        if (m_drumSamples[f]) delete m_drumSamples[f];
    };
    setSeq(0,0);
};

void CDrumMachine::setSeq( const unsigned char *seq, int seqLen ) {
    if (m_seq) delete [] m_seq;
    m_seq = 0;
    m_seqLen = 0;
    if (seq==0) return;
    m_seq = new unsigned char[seqLen];
    memcpy( m_seq, seq, seqLen );
    m_seqLen = seqLen;
};

void CDrumMachine::setBpm( int bpm ) {
    m_bpm = bpm;
    m_samplesPerTick = (AUDIO_FREQUENCY * 60) / m_bpm;
    m_sampleCounter = 0;

};


/**
 *
 * Run the drum machine.
 *
 */
void CDrumMachine::tick() {
    if (!m_seq) return;
    if (m_tickCount>=m_seqLen) m_tickCount = 0;
    unsigned char sbyte = m_seq[ m_tickCount ];

    int f;
    //int count = 0;
    //for (f=0; f<DRUM_MACHINE_SAMPLE_COUNT; f++) if (sbyte&(1<<f)) count++;
    //float setvol = 1.0f/(float)(count+1);
    float setvol = 1.0f;
    for (f=0; f<DRUM_MACHINE_SAMPLE_COUNT; f++) {
        if (sbyte&(1<<f)) {
            m_playInstances[f]->playBuffer( m_drumSamples[f],setvol, 1.0f );

        };
    }


    m_tickCount++;
};

int CDrumMachine::pullAudio( AUDIO_SAMPLE_TYPE *target, int length ) {

    m_mixer->setGeneralVolume( 2.5f / (float)DRUM_MACHINE_SAMPLE_COUNT );
    //m_mixer->setGeneralVolume(1.0f);
    int pos = 0;
    while (pos<length) {

        /*
        // time till next tick
        int nofActiveSamples = 0;
        for (int f=0; f<DRUM_MACHINE_SAMPLE_COUNT; f++) {
            if (m_playInstances[f]->isPlaying() == true) nofActiveSamples++;
        }
        */
        //m_mixer->setGeneralVolume( 0.9f / (float)(nofActiveSamples+1) );


        int sampleMixCount = ((length-pos)>>1);
        int samplesBeforeNextTick = m_samplesPerTick - m_sampleCounter;
        if (sampleMixCount>samplesBeforeNextTick) sampleMixCount = samplesBeforeNextTick;

        if (sampleMixCount>0) {
            int mixed = m_mixer->pullAudio( target, sampleMixCount*2 );
            if (mixed<1) return 0;              // fatal error
            pos+=mixed;
            target+=mixed;
            m_sampleCounter+=(mixed>>1);
        }

        if (m_sampleCounter>=m_samplesPerTick) {
            tick();
            m_sampleCounter-=m_samplesPerTick;
        };
    }

    return length;
};



