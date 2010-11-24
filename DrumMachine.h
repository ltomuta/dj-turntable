
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

    int getSeqLen() const { return m_seqLen; }
    int getSampleCount() const { return DRUM_MACHINE_SAMPLE_COUNT; }
    int getBpm() const { return m_bpm; }
    int getRunning() const { return m_running; }

    void setBpm(int bpm);
    void setSeq(const unsigned char *seq, int seqLen);
    void setRunning(bool running) { m_running = running; }

public slots:
    void startBeat() { setRunning(true); }
    void stopBeat() { setRunning(false); }
    void setBeatSpeed(int speed) { setBpm(speed); } // good values are anything between 300 and 800.
    void setDemoBeat(QVariant index);               // -1 index means that there are no beat at all. indexes 0-3 are the according presets

signals:
    // Describes the amount of ticks in sequence and count of samples
    void drumButtons(QVariant ticks, QVariant samples);

    // Describes the current tick on the sequence
    void tick(QVariant tick);

    // Describes the state of single DrumButton in sequence
    // index is 0 based
    void drumButton(QVariant index, QVariant pressed);

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
