
#ifndef __GE_IGA_INTERFACES__
#define __GE_IGA_INTERFACES__

#include <QObject>
#include <QMutex>

namespace GE {

#define AUDIO_FREQUENCY 44100//22050
#define AUDIO_SAMPLE_TYPE short
#define AUDIO_SAMPLE_BITS 16

    class IAudioSource : public QObject {
    public:
        IAudioSource();
        virtual ~IAudioSource();

        virtual int pullAudio(AUDIO_SAMPLE_TYPE *target,
                              int bufferLength ) = 0;
        virtual bool canBeDestroyed() { return false; }

        // For listing, do not touch if you dont know what you are doing.
        IAudioSource *m_next;
    };

    class CAudioMixer : public IAudioSource {
    public:
        CAudioMixer();
        virtual ~CAudioMixer();
        // Destroy all the sources in the list
        void destroyList();


        // Add new audio source to the list
        IAudioSource* addAudioSource(IAudioSource *s);
        int getAudioSourceCount();
        // Remove an audio source from the list
        bool removeAudioSource(IAudioSource *s);
        int pullAudio(AUDIO_SAMPLE_TYPE *target, int bufferLength);

        void setAbsoluteVolume(float vol);
        float getAbsoluteVolume() {
            return (float)m_fixedGeneralVolume / 4096.0f;
        }

        // Relative to the channelcount (audiosourcecount)
        void setGeneralVolume(float vol);
        float getGeneralVolume();

    protected:
        QMutex m_mutex;
        int m_fixedGeneralVolume;
        AUDIO_SAMPLE_TYPE *m_mixingBuffer;
        int m_mixingBufferLength;
        IAudioSource *m_sourceList;
    };
};

#endif
