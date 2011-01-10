#ifndef __TURNTABLE__
#define __TURNTABLE__

#include <QVariant>
#include <QPointer>

#include "ga_src/GEAudioOut.h"
#include "ga_src/GEAudioBuffer.h"

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5)
    #include <QSystemDeviceInfo>
    QTM_USE_NAMESPACE
#endif

#ifdef Q_OS_SYMBIAN
    #include <remconcoreapitargetobserver.h>
    class CRemConInterfaceSelector;
    class CRemConCoreApiTarget;
#endif


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

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5)
    void profile(QSystemDeviceInfo::Profile profile) {
        switch(profile) {
        case QSystemDeviceInfo::SilentProfile:
            m_audioMixer->setGeneralVolume(0.0f);
            break;
        default:
            break;
        }
    }
#endif

    int pullAudio(AUDIO_SAMPLE_TYPE *target, int bufferLength);

    void linkActivated(QVariant link);

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

#ifdef Q_OS_SYMBIAN

    class Observer : public MRemConCoreApiTargetObserver
    {
    public:
        Observer(TurnTable *turnTable) : m_TurnTable(turnTable) {}
        virtual void MrccatoCommand(TRemConCoreApiOperationId aOperationId,
                                    TRemConCoreApiButtonAction /*aButtonAct*/)
        {
            switch( aOperationId ) {
            case ERemConCoreApiVolumeDown:
                m_TurnTable->volumeDown();
                break;
            case ERemConCoreApiVolumeUp:
                m_TurnTable->volumeUp();
                break;
            default:
                break;
            }
        }
    protected:
        TurnTable *m_TurnTable;
    };

    Observer *m_Observer;
    CRemConInterfaceSelector *m_Selector;
    CRemConCoreApiTarget *m_Target;
#endif
};


#endif
