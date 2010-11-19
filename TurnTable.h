
#ifndef __CTURNTABLE__
#define __CTURNTABLE__

#include <QObject>
#include <QVariant>

#include "ga_src/GEAudioOut.h"
#include "ga_src/GEAudioBuffer.h"
#include "DrumMachine.h"


class CScratchDisc : public GE::IAudioSource {
public:
    CScratchDisc( GE::CAudioBuffer *discSource );
    virtual ~CScratchDisc();
    void setSpeed( float speed );
    int pullAudio( AUDIO_SAMPLE_TYPE *target, int bufferLength );

    inline void setHeadOn( bool set ) { m_headOn = set; }

protected:
    bool m_headOn;
    int m_volume;

    GE::CAudioBuffer *m_source;
    int m_pos;
    int m_cc;
    float m_speed;
    float m_targetSpeed;
};



class CTurnTable : public QObject {
    Q_OBJECT

public:
    CTurnTable();
    ~CTurnTable();

public slots:

    void setDiscSpeed( QVariant speed );

    void start();
    void stop() { m_sdisc->setHeadOn(false); }
    void startBeat( int index = 3 ) { toggleBeat(index); }
    void stopBeat() { toggleBeat(-1); }
    void setBeatSpeed( int speed ) { m_drumMachine->setBpm( speed ); } // good values are anything between 300 and 800.



    void toggleBeat( int index );               // -1 index means that there are no beat at all. indexes 0-3 are the according presets


protected:
    CScratchDisc *m_sdisc;
    CDrumMachine *m_drumMachine;

    GE::CAudioBufferPlayInstance *m_beatInstance;

    GE::AudioOut *m_audioOut;

    GE::CAudioMixer m_audioMixer;

    GE::CAudioBuffer *m_discSample;
};


#endif
