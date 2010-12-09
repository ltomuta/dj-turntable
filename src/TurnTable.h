#ifndef __CTURNTABLE__
#define __CTURNTABLE__

#include <QVariant>
#include <QPointer>
#include <QSystemDeviceInfo>

#include "ga_src/GEAudioOut.h"
#include "ga_src/GEAudioBuffer.h"

QTM_USE_NAMESPACE

class QSettings;


class TurnTable : public GE::IAudioSource
{
    Q_OBJECT

public:
    TurnTable(QSettings *settings);
    ~TurnTable();

    void addAudioSource(GE::IAudioSource *source);

public slots:

    void start() { m_headOn = true; }
    void stop() { m_headOn = false; }

    void setDiscAimSpeed(QVariant value);
    void setDiscSpeed(QVariant value);

    void setCutOff(QVariant value);
    void setResonance(QVariant value);

    void volumeUp();
    void volumeDown();

    void profile(QSystemDeviceInfo::Profile profile);

    int pullAudio(AUDIO_SAMPLE_TYPE *target, int bufferLength);

signals:
    void audioPosition(QVariant position);

protected:
    bool m_headOn;

    int m_loops;
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

    QSettings *m_Settings;

    QPointer<GE::CAudioBuffer> m_source;
    QPointer<GE::CAudioMixer> m_audioMixer;
    QPointer<GE::AudioOut> m_audioOut;
};


#endif
