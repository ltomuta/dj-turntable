
#ifndef __DRUMMACHINE__
#define __DRUMMACHINE__

#include <QObject>
#include <QVariant>
#include "ga_src/GEAudioBuffer.h"

#define DRUM_MACHINE_SAMPLE_COUNT 6

class CDrumMachine : public QObject, public GE::IAudioSource
{
    Q_OBJECT

public:
    CDrumMachine();
    virtual ~CDrumMachine();

    int pullAudio(AUDIO_SAMPLE_TYPE *target, int length);

    void setBpm(int bpm);
    void setSeq(const unsigned char *seq, int seqLen);
    void setRunning(bool running) { m_running = running; }

    void setMaxTickAndSamples(int ticks, int samples);

public slots:
    void startBeat() { setRunning(true); }
    void stopBeat() { setRunning(false); }
    void setBeatSpeed(int speed) { setBpm(speed); } // good values are anything between 300 and 800.
    void setDemoBeat(QVariant index);               // -1 index means that there are no beat at all. indexes 0-3 are the according presets
    void drumButtonToggled(QVariant tick, QVariant sample, QVariant pressed);

signals:
    // Describes to QML the maximum values of ticks and samples
    void maxSeqAndSamples(QVariant ticks, QVariant samples);

    // Describes the amount of ticks in sequence and count of samples
    void seqSize(QVariant ticks, QVariant samples);

    // Describes the current tick on the sequence
    void tickChanged(QVariant tick);

    // Describes the state of single DrumButton in sequence of the sample
    void drumButtonState(QVariant tick, QVariant sample, QVariant pressed);

protected:
    unsigned char *m_seq;
    int m_seqLen;

    void tick();

    bool m_running;
    int m_tickCount;
    int m_bpm;
    int m_samplesPerTick;
    int m_sampleCounter;

    GE::CAudioMixer *m_mixer;           // internal mixer
    GE::CAudioBuffer *m_drumSamples[DRUM_MACHINE_SAMPLE_COUNT];
    GE::CAudioBufferPlayInstance *m_playInstances[DRUM_MACHINE_SAMPLE_COUNT];
};

#endif
