/*
 * Copyright  2011 Nokia Corporation.
 */

#include <memory.h>
#include "GEInterfaces.h"

using namespace GE;

/**
 * AudioSource
 * common functionality
 *
 */
AudioSource::AudioSource(QObject *parent)
    : QObject(parent)
{
}


AudioSource::~AudioSource()
{
}


bool AudioSource::canBeDestroyed()
{
    return false;
}

/**
 * AudioMixer
 *
 */
AudioMixer::AudioMixer(QObject *parent)
    : AudioSource(parent)
{
    m_mixingBuffer = 0;
    m_mixingBufferLength = 0;
    m_fixedGeneralVolume = 4096;
}


AudioMixer::~AudioMixer()
{
    destroyList();

    if (m_mixingBuffer) {
        delete [] m_mixingBuffer;
        m_mixingBuffer = 0;
    }
}

// Destroy all the sources in the list.
void AudioMixer::destroyList()
{
    QMutexLocker locker(&m_mutex);

    QList<AudioSource*>::iterator it;
    for(it = m_sourceList.begin(); it != m_sourceList.end(); it++) {
        delete *it;
    }

    m_sourceList.clear();
}


// Add new audio source to the list. Return added source to caller.
AudioSource* AudioMixer::addAudioSource(AudioSource *source)
{
    QMutexLocker locker(&m_mutex);
    m_sourceList.push_back(source);
    return source;
}


// Remove a single audio source from the list
bool AudioMixer::removeAudioSource(AudioSource *source)
{
    QMutexLocker locker(&m_mutex);
    return m_sourceList.removeOne(source);
}


int AudioMixer::audioSourceCount()
{
    QMutexLocker locker(&m_mutex);
    return m_sourceList.count();
}


void AudioMixer::setGeneralVolume(float vol)
{
    m_fixedGeneralVolume = (4096.0f / (float)audioSourceCount() * vol);
}


// Relative to the channelcount (audiosourcecount)
float AudioMixer::generalVolume()
{
    return (float)m_fixedGeneralVolume *
           (float)audioSourceCount() / 4096.0f;
}


void AudioMixer::setAbsoluteVolume(float vol)
{
    m_fixedGeneralVolume = (4096.0f * vol);
}


float AudioMixer::absoluteVolume()
{
    return (float)m_fixedGeneralVolume / 4096.0f;
}


int AudioMixer::pullAudio(AUDIO_SAMPLE_TYPE *target, int bufferLength)
{
    QMutexLocker locker(&m_mutex);

    if(m_sourceList.isEmpty())
        return 0;

    if (m_mixingBufferLength < bufferLength) {
        if (m_mixingBuffer)
            delete [] m_mixingBuffer;

        m_mixingBufferLength = bufferLength;
        m_mixingBuffer = new AUDIO_SAMPLE_TYPE[m_mixingBufferLength];
    };

    memset(target, 0, sizeof(AUDIO_SAMPLE_TYPE) * bufferLength);

    AUDIO_SAMPLE_TYPE *t;
    AUDIO_SAMPLE_TYPE *t_target;
    AUDIO_SAMPLE_TYPE *s;

    QList<AudioSource*>::iterator it = m_sourceList.begin();
    while (it != m_sourceList.end()) {
        int mixed = (*it)->pullAudio(m_mixingBuffer, bufferLength);
        if(mixed > 0) {
            // mix to main..
            t = target;
            t_target = t + mixed;
            s = m_mixingBuffer;
            while (t != t_target) {
                *t += (((*s) * m_fixedGeneralVolume) >> 12);
                t++;
                s++;
            }
        }

        if((*it)->canBeDestroyed()) {
            // autodestroy
            // NOTE, IS UNDER TESTING,... MIGHT CAUSE UNPREDICTABLE CRASHING
            // WITH SOME USE CASES!!!
            delete *it;
            it = m_sourceList.erase(it);
        }
        else {
            it++;
        }
    }

    return bufferLength;
}
