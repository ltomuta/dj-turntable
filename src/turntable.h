/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

#ifndef __TURNTABLE__
#define __TURNTABLE__

#include <QMutex>
#include <QVariant>
#include <QPointer>

#include "audioout.h"
#include "audiobuffer.h"
#include "audiomixer.h"
#include "cutoffeffect.h"
#include "vorbisdecoder.h"

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    #include <QSystemDeviceInfo>
    QTM_USE_NAMESPACE
#endif

#if defined(Q_OS_SYMBIAN) && !defined(Q_OS_SYMBIAN_1)
    #include "volumekeys.h"
#endif

class QSettings;


class Turntable : public GE::AudioSource
#if defined(Q_OS_SYMBIAN) && !defined(Q_OS_SYMBIAN_1)
                 ,public GE::IVolumeKeyObserver
#endif
{
    Q_OBJECT

public:
    Turntable(QSettings *settings, QObject *parent = 0);
    ~Turntable();

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
    void profile(QSystemDeviceInfo::Profile profile);
#endif

    int pullAudio(AUDIO_SAMPLE_TYPE *target, int bufferLength);

signals:
    void sampleOpened(QVariant filePath);
    void audioPosition(QVariant position);
    void error(QVariant file, QVariant error);

    void powerOn();
    void powerOff();

protected:
    QSettings *m_Settings; // Not owned
    GE::AudioBuffer* m_buffer; // Owned
    GE::VorbisDecoder *m_decoder; // Owned
    GE::AudioMixer* m_audioMixer; // Owned
    GE::AudioOut* m_audioOut; // Owned
    GE::CutOffEffect *m_cutOffEffect; // Owned
    QMutex m_PosMutex;
    const QString m_defaultSample;
    const float m_defaultVolume;
    bool m_headOn;
    const int m_maxLoops;
    int m_loops;
    qint64 m_pos;
    int m_cc;
    float m_speed;
    float m_targetSpeed;
    qint64 m_channelLength;

#if defined(Q_OS_SYMBIAN) && !defined(Q_OS_SYMBIAN_1)
    // To handle the hardware volume keys on Symbian
    GE::VolumeKeys *m_volumeKeys; // Owned
#endif
};

#endif
