
#include <QtGui>
#include <vector>
#include <limits>
#include "DrumMachine.h"

using namespace GE;

// Predefined sequences, they must be SEQUENCE_LENGTH ticks long each
const unsigned char drum_seq0[] = { 5, 0, 1, 0, 9, 0, 5, 0,
                                    5, 0, 1, 0, 9, 4, 2, 0,
                                    5, 0, 1, 0, 9, 0, 1, 4,
                                    1, 4, 1, 0, 9, 0, 2, 0};

const unsigned char drum_seq1[] = { 5, 0, 5, 0, 2, 0, 0, 0,
                                    1, 0, 1, 0, 2, 0, 0, 0,
                                    5, 0, 1, 0, 2, 0, 0, 0,
                                    1, 0, 1, 0, 2, 0, 1, 0};

const unsigned char drum_seq2[] = { 5, 0, 1, 0,33, 0, 5, 0,
                                    5, 0, 1, 0,33, 0, 2, 0,
                                    5, 0, 1, 0,33, 0, 5, 0,
                                    5, 0, 1, 0,33, 0, 2, 0};


DrumMachine::DrumMachine(QSettings *settings, QObject *parent)
    : GE::AudioSource(parent),
      m_tickCount(0),
      m_Settings(settings),
      m_running(false),
      m_samplesPerTick(0),
      m_sampleCounter(0),
      m_currentSeqIndex(-1)
{
    m_mixer = new AudioMixer;

    m_drumSamples << AudioBuffer::loadWav(QString(":/sounds/hihat.wav"))
                  << AudioBuffer::loadWav(QString(":/sounds/hihat_open.wav"))
                  << AudioBuffer::loadWav(QString(":/sounds/bassd.wav"))
                  << AudioBuffer::loadWav(QString(":/sounds/snare.wav"))
                  << AudioBuffer::loadWav(QString(":/sounds/cymbal.wav"))
                  << AudioBuffer::loadWav(QString(":/sounds/cowbell.wav"));

    for (int i=0; i<m_drumSamples.size(); i++) {
        AudioBufferPlayInstance *playInstance = new AudioBufferPlayInstance;
        // Dont destroy object when playing is finished
        playInstance->setDestroyWhenFinished(false);
        m_mixer->addAudioSource(playInstance);
        m_playInstances.push_back(playInstance);
    }

    m_speedMultiplier = 1.0f;
    m_mixer->setAbsoluteVolume(3.0f / m_drumSamples.size());

    setBpm(600);
}


DrumMachine::~DrumMachine()
{
    foreach (GE::AudioBufferPlayInstance* playInstance, m_playInstances) {
        m_mixer->removeAudioSource(playInstance);
        delete playInstance;
    }

    foreach (GE::AudioBuffer* sample, m_drumSamples) {
        delete sample;
    }

    delete m_mixer;
}


/**
 *
 * Returns copy of current sequence
 *
 */
DrumMachine::TYPE_DRUM_SEQ DrumMachine::seq() const
{
    return m_seq;
}


void DrumMachine::setSeq(const TYPE_DRUM_SEQ &seq)
{
    m_seq = seq;
}


int DrumMachine::bpm() const
{
    return (AUDIO_FREQUENCY * 60) / m_samplesPerTick;
}


void DrumMachine::setSpeedMultiplier( float speedMul ) {
    m_speedMultiplier = speedMul;
}


void DrumMachine::setBpm(int bpm)
{
    float samplesPerTick = std::numeric_limits<float>::max();

    if (bpm != 0) {
        samplesPerTick  = (float)(AUDIO_FREQUENCY * 60.0f *
                                  m_speedMultiplier ) / (bpm * 1.0);
    }

    m_samplesPerTick = (int)samplesPerTick;
}


bool DrumMachine::isUserBeat() const
{
    if (m_currentSeqIndex >= 3 && m_currentSeqIndex <= 5) {
        return true;
    }

    return false;
}


/**
 *
 * Run the drum machine.
 *
 */
void DrumMachine::tick()
{
    if (!m_running || m_seq.empty()) {
        return;
    }

    if (m_tickCount >= m_seq.size()) {
        m_tickCount = 0;
    }
    unsigned char sbyte = m_seq[m_tickCount];

    float setvol = 1.0f;
    for (int f=0; f<m_drumSamples.size(); f++) {
        if (sbyte & (1 << f)) {
            m_playInstances[f]->playBuffer(m_drumSamples[f], setvol, 1.0f);
        }
    }

    emit tickChanged(m_tickCount);

    m_tickCount++;
}


int DrumMachine::pullAudio(AUDIO_SAMPLE_TYPE *target, int length)
{
    int pos = 0;
    while (pos < length) {
        int sampleMixCount = ((length-pos) >> 1);
        int samplesBeforeNextTick = m_samplesPerTick - m_sampleCounter;

        if (sampleMixCount > samplesBeforeNextTick) {
            sampleMixCount = samplesBeforeNextTick;
        }

        if (sampleMixCount > 0) {
            int mixed = m_mixer->pullAudio(target, sampleMixCount * 2);
            if (mixed < 1) {
                // fatal error
                return 0;
            }
            pos += mixed;
            target += mixed;
            m_sampleCounter += (mixed >> 1);
        }

        if (m_sampleCounter >= m_samplesPerTick) {
            tick();
            m_sampleCounter -= m_samplesPerTick;
        }
    }

    return length;
}


DrumMachine::TYPE_DRUM_SEQ DrumMachine::readUserBeat(int index)
{
    DrumMachine::TYPE_DRUM_SEQ seq;

    QString key = QString("UserBeat_%1").arg(index);
    QStringList list = m_Settings->value(key).toString().split(',');
    if (list.size() != SEQUENCE_LENGTH) {
        // There was no user saved beat yet or the beat was corrupted,
        // create an empty 32 item seq.
        seq.fill(0, SEQUENCE_LENGTH);
        return seq;
    }

    QStringList::const_iterator it;
    for (it=list.begin(); it!=list.end(); it++) {
        seq.push_back(it->toULong());
    }

    return seq;
}


void DrumMachine::saveUserBeat(int index,
                               const DrumMachine::TYPE_DRUM_SEQ &seq)
{
    QString key = QString("UserBeat_%1").arg(index);
    QString data;

    DrumMachine::TYPE_DRUM_SEQ::const_iterator it;

    for (it=seq.begin(); it != seq.end(); it++) {
        data += QString("%1").arg(*it);
        if (it + 1 != seq.end())
            data += ",";
    }

    m_Settings->setValue(key, data);
}



void DrumMachine::setBeat(QVariant index)
{
    // We use STL vector to get predefined beats
    // easily from hard coded arrays to QVector
    std::vector<unsigned char> tempvec;

    switch (index.toInt()) {
    // Predefined sequences
    //
    case 0:
        tempvec.assign(drum_seq0, drum_seq0 + SEQUENCE_LENGTH);
        m_seq = QVector<unsigned char>::fromStdVector(tempvec);
        break;
    case 1:
        tempvec.assign(drum_seq1, drum_seq1 + SEQUENCE_LENGTH);
        m_seq = QVector<unsigned char>::fromStdVector(tempvec);
        break;
    case 2:
        tempvec.assign(drum_seq2, drum_seq2 + SEQUENCE_LENGTH);
        m_seq = QVector<unsigned char>::fromStdVector(tempvec);
        break;
        // User defined sequences
        //
    case 3:
    case 4:
    case 5:
        m_seq = readUserBeat(index.toInt());
        break;

        // Invalid index, do nothing
        //
    default:
        return;
    };

    // Update the UI with new drum sequence
    //
    for (unsigned char i=0; i<m_seq.size(); i++) {
        unsigned char tick = m_seq[i];
        for (int j=0; j<m_drumSamples.size(); j++) {
            if (tick & 1)
                emit(drumButtonState(i, j, true));
            else
                emit(drumButtonState(i, j, false));
            tick = tick >> 1;
        }
    }

    m_currentSeqIndex = index.toInt();
}


/**
 *
 * User has changed the sequence, edit the change directly to the playing beat
 *
 */
void DrumMachine::drumButtonToggled(QVariant tick, QVariant sample,
                                    QVariant pressed)
{
    unsigned char iTick(tick.toUInt());
    int iSample(sample.toInt());
    bool bPressed(pressed.toBool());

    if (iTick >= m_seq.size()) {
        // UI thinks the sequence is longer than the model
        // There is nothing we can do.
        return;
    }

    if (bPressed)
        m_seq[iTick] = m_seq[iTick] | (1 << iSample);
    else
        m_seq[iTick] = m_seq[iTick] ^ (1 << iSample);

    if (isUserBeat() == false) {
        // User edited predefined beats, don't save
        return;
    }

    saveUserBeat(currentSeqIndex(), m_seq);
}
