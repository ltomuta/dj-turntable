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

    bool headOn() const { return m_headOn; }
    float speed() const { return m_speed; }
    float cutOff() const { return m_cutOffValue; }

    void setHeadOn(bool set) { m_headOn = set; }
    void setSpeed(float speed);
    // guides to speed towards attribute speed with a power of power (0-1), good values are below 0.1
    void aimSpeed(float speed, float power = 0.05f);
    void setCutOff(float cutoff);
    void setResonance(float resonance);

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
    float m_cutOffTarget;
    float m_resonanceTarget;

    // Filters
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
    void setDiscAimSpeed(QVariant speed);
    void setDiscSpeed(QVariant speed);

    void start() { m_sdisc->setHeadOn(true); }
    void stop() { m_sdisc->setHeadOn(false); }

    void cutOff(QVariant value) { m_sdisc->setCutOff(value.toFloat()); }
    void resonance(QVariant value) { m_sdisc->setResonance(value.toFloat()); }

protected:
    CScratchDisc *m_sdisc;

    GE::AudioOut *m_audioOut;
    GE::CAudioMixer m_audioMixer;
    GE::CAudioBuffer *m_discSample;
};


#endif
