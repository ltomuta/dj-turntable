/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

#ifndef __DRUMMACHINE__
#define __DRUMMACHINE__

#include <QVariant>
#include <QVector>
#include <QPointer>
#include "audiomixer.h"
#include "audiobufferplayinstance.h"
#include "audiobuffer.h"

class QSettings;

// Shortens writing of the type later on
typedef QVector<unsigned char> TYPE_DRUM_SEQ;

class DrumMachine : public GE::AudioSource
{
    Q_OBJECT

public:
    DrumMachine(QSettings *settings, QObject *parent = 0);
    virtual ~DrumMachine();

    int bpm() const;
    TYPE_DRUM_SEQ seq() const;
    int currentSeqIndex() const { return m_currentSeqIndex; }

    // You must call setBpm after this one.
    void setSpeedMultiplier(float speedMul = 1.0f);
    void setBpm(int bpm);
    void setSeq(const TYPE_DRUM_SEQ &seq);
    void setRunning(bool running) { m_running = running; }

    // Returns true if beat index is in range 4-7
    bool isUserBeat() const;

    int pullAudio(AUDIO_SAMPLE_TYPE *target, int length);

public slots:
    void startBeat() { setRunning(true); }
    void stopBeat() { setRunning(false); }
    void setBeatSpeed(QVariant speed);
    void setBeat(QVariant index);
    void drumButtonToggled(QVariant tick, QVariant sample, QVariant pressed);

signals:
    // Describes the current tick on the sequence
    void tickChanged(QVariant tick);

    // Describes the state of single DrumButton in sequence of the sample
    void drumButtonState(QVariant tick, QVariant sample, QVariant pressed);

private:
    void tick();
    TYPE_DRUM_SEQ readUserBeat(int index);
    void saveUserBeat(int index, const TYPE_DRUM_SEQ &seq);

private:
    QSettings *m_Settings; // Not owned
    QPointer<GE::AudioMixer> m_mixer; // Not owned
    QVector<GE::AudioBufferPlayInstance*> m_playInstances; // Owned
    QVector<GE::AudioBuffer*> m_drumSamples; // Owned

    TYPE_DRUM_SEQ m_seq;
    TYPE_DRUM_SEQ::size_type m_tickCount;

    float m_speedMultiplier;
    bool m_running;
    int m_samplesPerTick;
    int m_sampleCounter;
    int m_currentSeqIndex;
};

#endif
