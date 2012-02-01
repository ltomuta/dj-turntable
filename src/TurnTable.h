#ifndef __TURNTABLE__
#define __TURNTABLE__

#include <QVariant>
#include <QPointer>
#include <QMutex>

#include "audioout.h"
#include "audiomixer.h"
#include "audiobuffer.h"
#include "cutoffeffect.h"
#include "vorbissource.h"
#include "vorbisdecoder.h"

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    #include <QSystemDeviceInfo>
    QTM_USE_NAMESPACE
#endif

#ifdef Q_OS_SYMBIAN
    #include <remconcoreapitargetobserver.h>
    class CRemConInterfaceSelector;
    class CRemConCoreApiTarget;
#endif


class QSettings;


class TurnTable : public GE::AudioSource
{
    Q_OBJECT

public:
    TurnTable(QSettings *settings, QObject *parent = 0);
    ~TurnTable();

    void addAudioSource(GE::AudioSource *source);
    void openSample(const QString &filePath = "");
    void openLastSample();

public slots:

    void setSample(QVariant value);
    void openDefaultSample();

    void start() { m_headOn = true; }
    void stop() { m_headOn = false; }

    void setDiscAimSpeed(QVariant value);
    void setDiscSpeed(QVariant value);

    void setCutOff(QVariant value);
    void setResonance(QVariant value);

    void volumeUp();
    void volumeDown();

    void seekToPosition(QVariant position);

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    void profile(QSystemDeviceInfo::Profile profile) {
        if (profile == QSystemDeviceInfo::SilentProfile) {
            m_audioMixer->setGeneralVolume(0.0f);
        }
    #if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
        // In Maemo where there is no way to get volume
        // back if it is set to 0, we set the volume
        // to the default volume when getting out of
        // silent profile. In Maemo the devices volume
        // buttons control the devices volume, in Symbian
        // the devices volume buttons control application
        // specific volume.
        else {
            m_audioMixer->setGeneralVolume(m_defaultVolume);
        }
    #endif
    }
#endif

    int pullAudio(AUDIO_SAMPLE_TYPE *target, int bufferLength);

signals:
    void sampleOpened(QVariant filePath);
    void audioPosition(QVariant position);
    void error(QVariant file, QVariant error);

    void powerOn();
    void powerOff();

protected:

    const QString m_defaultSample;
    const float m_defaultVolume;

    bool m_headOn;

    const int m_maxLoops;
    int m_loops;
    long long int m_pos;
    int m_cc;
    float m_speed;
    float m_targetSpeed;

    QMutex m_PosMutex;

    QSettings *m_Settings;

    GE::AudioBuffer* m_buffer;
    GE::VorbisDecoder *m_decoder;
    GE::VorbisSource* m_vorbis;
    GE::AudioMixer* m_audioMixer;
    GE::AudioOut* m_audioOut;
    GE::CutOffEffect *m_cutOffEffect;

#ifdef Q_OS_SYMBIAN
    // To handle the hardware volume keys on Symbian
    class Observer : public MRemConCoreApiTargetObserver
    {
    public:
        Observer(TurnTable *turnTable) : m_TurnTable(turnTable) {}
        virtual void MrccatoCommand(TRemConCoreApiOperationId aOperationId,
                                    TRemConCoreApiButtonAction /*aButtonAct*/)
        {
            switch ( aOperationId ) {
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
