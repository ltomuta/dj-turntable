#ifndef __DRUMMACHINE__
#define __DRUMMACHINE__

#include <QSettings>
#include <QVariant>
#include <QVector>
#include "ga_src/GEAudioBuffer.h"

class CDrumMachine : public QObject, public GE::IAudioSource
{
    Q_OBJECT

public:
    enum { SEQUENCE_LENGTH = 32 };

    // Shortens writing of the type later on
    typedef QVector<unsigned char> TYPE_DRUM_SEQ;

    CDrumMachine();
    virtual ~CDrumMachine();

    int bpm() const;
    TYPE_DRUM_SEQ seq() const;
    int currentSeqIndex() const { return m_currentSeqIndex; }

    void setBpm(int bpm);
    void setSeq(const TYPE_DRUM_SEQ &seq);
    void setRunning(bool running) { m_running = running; }
    void setMaxTickAndSamples(int ticks, int samples);

    // Returns true if beat index is in range 4-7
    bool isUserBeat() const;

    // Called when more is needed
    int pullAudio(AUDIO_SAMPLE_TYPE *target, int length);



public slots:
    void startBeat() { setRunning(true); }
    void stopBeat() { setRunning(false); }

    // Sets the beats speep
    void setBeatSpeed(QVariant speed) { setBpm(speed.toInt()); }

    // Indexes 0-3 are predefined beats, 4-7 are user defined
    void setBeat(QVariant index);

    // Sets / unsets the drum the corresponding tick and sample
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

    TYPE_DRUM_SEQ m_seq;
    TYPE_DRUM_SEQ::size_type m_tickCount;

    QSettings m_Settings;

    bool m_running;
    int m_samplesPerTick;
    int m_sampleCounter;
    int m_currentSeqIndex;

    GE::CAudioMixer *m_mixer;
    QVector<GE::CAudioBufferPlayInstance*> m_playInstances;
    QVector<GE::CAudioBuffer*> m_drumSamples;

    // Called when the sequence advances a tick
    void tick();

    TYPE_DRUM_SEQ readUserBeat(int index);
    void saveUserBeat(int index, const TYPE_DRUM_SEQ &seq);
};

#endif
