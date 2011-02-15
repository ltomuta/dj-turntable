/*
 * Copyright  2011 Nokia Corporation.
 */

#ifndef __GE_IGA_INTERFACES__
#define __GE_IGA_INTERFACES__

#include <QObject>
#include <QMutex>
#include <QList>

namespace GE {

#define AUDIO_FREQUENCY 44100
#define AUDIO_SAMPLE_TYPE short
#define AUDIO_SAMPLE_BITS 16

    class AudioSource : public QObject {
    public:
        AudioSource(QObject *parent = 0);
        virtual ~AudioSource();

        virtual int pullAudio(AUDIO_SAMPLE_TYPE *target,
                              int bufferLength ) = 0;

        virtual bool canBeDestroyed();
    };

    class AudioMixer : public AudioSource {
    public:
        AudioMixer(QObject *parent = 0);
        virtual ~AudioMixer();

        AudioSource* addAudioSource( AudioSource *s );
        bool removeAudioSource( AudioSource *s );
        int audioSourceCount();
        void destroyList();

        int pullAudio(AUDIO_SAMPLE_TYPE *target, int bufferLength);

        void setAbsoluteVolume(float vol);
        float absoluteVolume();

        void setGeneralVolume(float vol);
        float generalVolume();
    protected:

        QList<AudioSource*> m_sourceList;
        QMutex m_mutex;
        int m_fixedGeneralVolume;
        AUDIO_SAMPLE_TYPE *m_mixingBuffer;
        int m_mixingBufferLength;
    };
};

#endif
