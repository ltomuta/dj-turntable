
/*
 * Copyright  2011 Nokia Corporation.
 *
 *
 */
#ifndef __GE_IGA_AUDIOBUFFER__
#define __GE_IGA_AUDIOBUFFER__


#include <QString>
#include <QFile>
#include "GEInterfaces.h"


namespace GE {

    class AudioBufferPlayInstance;
    class AudioBuffer;
    // Prototype function for audio sampling
    typedef AUDIO_SAMPLE_TYPE(*SAMPLE_FUNCTION_TYPE)(AudioBuffer *abuffer,
                                                     int pos,
                                                     int channel);

/*
 * Class to hold audio information (a buffer).
 *
 */
    class AudioBuffer : public QObject {
    public:
        AudioBuffer();
        virtual ~AudioBuffer();

        static AudioBuffer* loadWav(const QString &fileName,
                                    QString *errorString = 0);
        static AudioBuffer* loadWav(FILE *wavFile,
                                    QString *errorString = 0);
        static AudioBuffer* loadWav(QFile &wavFile,
                                    QString *errorString = 0);

        void reallocate( int length );

        inline void* rawData() { return m_data; }
        inline int dataLength() { return m_dataLength; }

        inline int bytesPerSample() { return (m_bitsPerSample >> 3); }
        inline int bitsPerSample() { return m_bitsPerSample; }
        inline int samplesPerSec() { return m_samplesPerSec; }
        inline short nofChannels() { return m_nofChannels; }
        inline SAMPLE_FUNCTION_TYPE sampleFunction() {
            return m_sampleFunction;
        }

        // Static implementations of sample functions
        static AUDIO_SAMPLE_TYPE sampleFunction8bitMono(
            AudioBuffer *abuffer, int pos, int channel);
        static AUDIO_SAMPLE_TYPE sampleFunction16bitMono(
            AudioBuffer *abuffer, int pos, int channel);
        static AUDIO_SAMPLE_TYPE sampleFunction32bitMono(
            AudioBuffer *abuffer, int pos, int channel);
        static AUDIO_SAMPLE_TYPE sampleFunction8bitStereo(
            AudioBuffer *abuffer, int pos, int channel);
        static AUDIO_SAMPLE_TYPE sampleFunction16bitStereo(
            AudioBuffer *abuffer, int pos, int channel);
        static AUDIO_SAMPLE_TYPE sampleFunction32bitStereo(
            AudioBuffer *abuffer, int pos, int channel);

        AudioBufferPlayInstance *playWithMixer(GE::AudioMixer &mixer);

    protected:
        SAMPLE_FUNCTION_TYPE m_sampleFunction;
        short m_nofChannels;
        void *m_data;
        int m_dataLength; // in bytes
        short m_bitsPerSample;
        bool m_signedData;
        int m_samplesPerSec;
    };


    class AudioBufferPlayInstance : public AudioSource {
    public:
        AudioBufferPlayInstance();
        AudioBufferPlayInstance(AudioBuffer *start_playing);
        virtual ~AudioBufferPlayInstance();
        // Looptimes -1 = loop forever
        void playBuffer(AudioBuffer *startPlaying, float volume,
                        float fixedSpeed, int loopTimes = 0);

        void setSpeed(float speed);
        void setLeftVolume(float lvol);
        void setRightVolume(float rvol);

        inline void setLoopTimes(int ltimes) { m_loopTimes = ltimes; }
        void stop();

        int pullAudio(AUDIO_SAMPLE_TYPE *target, int bufferLength);
        bool canBeDestroyed();

        bool isPlaying() {
            if (m_buffer)
                return true;
            else
                return false;
        }
        inline bool isFinished() { return m_finished; }
        inline bool destroyWhenFinished() { return m_destroyWhenFinished; }
        inline void setDestroyWhenFinished( bool set ) {
            m_destroyWhenFinished = set;
        }

    protected:
        int mixBlock( AUDIO_SAMPLE_TYPE *target, int bufferLength );
        bool m_finished;
        bool m_destroyWhenFinished;
        int m_fixedPos;
        int m_fixedInc;

        int m_fixedLeftVolume;
        int m_fixedRightVolume;
        int m_fixedCenter;
        int m_loopTimes;

        AudioBuffer *m_buffer;
    };
};



#endif
