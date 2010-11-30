#ifndef __CTURNTABLE__
#define __CTURNTABLE__

#include <QObject>
#include <QVariant>

#include "ga_src/GEAudioOut.h"
#include "ga_src/GEAudioBuffer.h"


class CScratchDisc : public GE::IAudioSource
{
public:
    CScratchDisc(GE::CAudioBuffer *discSource);
    virtual ~CScratchDisc();

    float getSpeed() const { return m_speed; }
    bool getHeadOn() const { return m_headOn; }

    void setSpeed(float speed);
    void setHeadOn(bool set) { m_headOn = set; }


    void setCutoff( float cutoff ) { m_cutOffValue = cutoff; }
    void setResonance( float resonance ) { m_resonanceValue = resonance; }

    int pullAudio(AUDIO_SAMPLE_TYPE *target, int bufferLength);

protected:
    bool m_headOn;
    int m_volume;

    GE::CAudioBuffer *m_source;
    int m_pos;
    int m_cc;
    float m_speed;
    float m_targetSpeed;

    float m_cutOffValue;
    float m_resonanceValue;

        // filters
    int m_lp[2];
    int m_hp[2];
    int m_bp[2];
};


class TurnTable : public QObject
{
    Q_OBJECT

public:
    TurnTable();
    ~TurnTable();

    void addAudioSource(GE::IAudioSource *source);

public slots:
    void setDiscSpeed(QVariant speed);
    void start() { m_sdisc->setHeadOn(true); }
    void stop() { m_sdisc->setHeadOn(false); }
    void cutoff(QVariant value) { m_sdisc->setCutoff(value.toFloat()); }
    void resonance(QVariant value) { m_sdisc->setResonance(value.toFloat()); }

protected:
    CScratchDisc *m_sdisc;

    GE::AudioOut *m_audioOut;
    GE::CAudioMixer m_audioMixer;
    GE::CAudioBuffer *m_discSample;
};


#endif
