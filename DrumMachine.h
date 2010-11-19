
#ifndef __DRUMMACHINE__
#define __DRUMMACHINE__

#include "ga_src/GEAudioBuffer.h"


#define DRUM_MACHINE_SAMPLE_COUNT 6


class CDrumMachine : public GE::IAudioSource {
public:
    CDrumMachine();
    virtual ~CDrumMachine();

    int pullAudio( AUDIO_SAMPLE_TYPE *target, int length );

        // change the speed of the machine
    inline int getBpm() { return m_bpm; }
    void setBpm( int bpm );

    void setSeq( const unsigned char *seq, int seqLen );

protected:
    unsigned char *m_seq;
    int m_seqLen;

    void tick();

    int m_tickCount;
    int m_bpm;
    int m_samplesPerTick;
    int m_sampleCounter;

    GE::CAudioMixer *m_mixer;           // internal mixer
    GE::CAudioBuffer *m_drumSamples[ DRUM_MACHINE_SAMPLE_COUNT ];
    GE::CAudioBufferPlayInstance *m_playInstances[ DRUM_MACHINE_SAMPLE_COUNT ];
};



#endif
