#ifndef __DRUMMACHINE__
#define __DRUMMACHINE__

#include <QVariant>
#include <QVector>
#include <QPointer>
#include "ga_src/GEAudioBuffer.h"

class QSettings;

class DrumMachine : public GE::IAudioSource
{
    Q_OBJECT

public:
    enum { SEQUENCE_LENGTH = 32 };

    // Shortens writing of the type later on
    typedef QVector<unsigned char> TYPE_DRUM_SEQ;

    DrumMachine(QSettings *settings);
    virtual ~DrumMachine();

    int bpm() const;
    TYPE_DRUM_SEQ seq() const;
    int currentSeqIndex() const { return m_currentSeqIndex; }

    void setSpeedMultiplier( float speedMul = 1.0f );           // you must call setBpm after this one.
    void setBpm(int bpm);
    void setSeq(const TYPE_DRUM_SEQ &seq);
    void setRunning(bool running) { m_running = running; }

    // Returns true if beat index is in range 4-7
    bool isUserBeat() const;

    // Called when more is needed
    int pullAudio(AUDIO_SAMPLE_TYPE *target, int length);



public slots:
    void startBeat() { setRunning(true); }
    void stopBeat() { setRunning(false); }

    // Sets the beats speep
    void setBeatSpeed(QVariant speed) { setBpm(speed.toFloat() * 600); }

    // Indexes 0-3 are predefined beats, 4-7 are user defined
    void setBeat(QVariant index);

    // Sets / unsets the drum the corresponding tick and sample
    void drumButtonToggled(QVariant tick, QVariant sample, QVariant pressed);



signals:
    // Describes the current tick on the sequence
    void tickChanged(QVariant tick);

    // Describes the state of single DrumButton in sequence of the sample
    void drumButtonState(QVariant tick, QVariant sample, QVariant pressed);


protected:

    TYPE_DRUM_SEQ m_seq;
    TYPE_DRUM_SEQ::size_type m_tickCount;

    QSettings *m_Settings;

    float m_speedMultiplier;
    bool m_running;
    int m_samplesPerTick;
    int m_sampleCounter;
    int m_currentSeqIndex;

    QPointer<GE::CAudioMixer> m_mixer;
    QVector<GE::CAudioBufferPlayInstance*> m_playInstances;
    QVector<GE::CAudioBuffer*> m_drumSamples;

    // Called when the sequence advances a tick
    void tick();

    TYPE_DRUM_SEQ readUserBeat(int index);
    void saveUserBeat(int index, const TYPE_DRUM_SEQ &seq);
};

#endif
