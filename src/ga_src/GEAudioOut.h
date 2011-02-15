/*
 * Copyright  2011 Nokia Corporation.
 *
 */

#ifndef __GE_QTAUDIOOUT__
#define __GE_QTAUDIOOUT__

#include <QThread>
#include "GEInterfaces.h"


class QAudioOutput;
class QIODevice;

namespace GE {

    class AudioOut : public QThread {
        Q_OBJECT

    public:
        AudioOut(QObject *parent, GE::AudioSource *source);
        virtual ~AudioOut();

    /*
     * call this manually only if you are not using thread(with Symbian)
     * Note, when using GE, windowwg owning the audioout will handle of
     * calling this.
     */
    void tick();

    private slots:
        // For internal notify "solution"
        void audioNotify();

    protected:
        // This is for the threaded mode only
        virtual void run();

        qint64 m_samplesMixed;

        QAudioOutput *m_audioOutput;
        QIODevice *m_outTarget;
        AudioSource *m_source;
        int m_runstate;
        AUDIO_SAMPLE_TYPE *m_sendBuffer;
        int m_sendBufferSize;
    };
}

#endif
