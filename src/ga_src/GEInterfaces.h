/*
 * Copyright  2011 Nokia Corporation.
 */

#ifndef __GE_IGA_INTERFACES__
#define __GE_IGA_INTERFACES__

#include <QObject>
#include <QMutex>

namespace GE {

#define AUDIO_FREQUENCY 44100
#define AUDIO_SAMPLE_TYPE short
#define AUDIO_SAMPLE_BITS 16

    class IAudioSource : public QObject {
    public:
        IAudioSource(QObject *parent = 0);
        virtual ~IAudioSource();

        virtual int pullAudio(AUDIO_SAMPLE_TYPE *target,
                              int bufferLength ) = 0;
        virtual bool canBeDestroyed() { return false; }

        // For listing, do not touch if you dont know what you are doing.
        IAudioSource *m_next;
    };

    class CAudioMixer : public IAudioSource {
    public:
        CAudioMixer(QObject *parent = 0);
        virtual ~CAudioMixer();
	bool enabled() { return m_enabled; }
	void setEnabled( bool set ) { m_enabled = set; }

        // Destroy all the sources in the list.
        void destroyList();

        // Add new audio source to the list. Return added source to caller.
        IAudioSource* addAudioSource( IAudioSource *s );
        int getAudioSourceCount();

        // Remove a single audio source from the list
        bool removeAudioSource( IAudioSource *s );

        int pullAudio(AUDIO_SAMPLE_TYPE *target, int bufferLength);

        void setAbsoluteVolume(float vol);
        float getAbsoluteVolume() {
            return (float)m_fixedGeneralVolume / 4096.0f;
        }

        // Relative to the channelcount (audiosourcecount)
        void setGeneralVolume(float vol);
        float getGeneralVolume();

    protected:
	bool m_enabled;
        QMutex m_mutex;
        int m_fixedGeneralVolume;
        AUDIO_SAMPLE_TYPE *m_mixingBuffer;
        int m_mixingBufferLength;
        IAudioSource *m_sourceList;
    };
};

#endif
