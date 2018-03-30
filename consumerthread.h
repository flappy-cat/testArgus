#ifndef CONSUMERTHREAD_H
#define CONSUMERTHREAD_H
 #include<libcams.h>
#include<time.h>
using namespace std;
using namespace Argus;
using namespace EGLStream;

class ConsumerThread:public ArgusSamples::Thread{
public:
    explicit ConsumerThread(OutputStream* stream);
    ~ConsumerThread();

    bool isInError()
    {
        return m_gotError;
    }

private:
    /** @name Thread methods */
    /**@{*/
    virtual bool threadInitialize();
    virtual bool threadExecute();
    virtual bool threadShutdown();
    /**@}*/

    bool createVideoEncoder();
    void abort();

    static bool encoderCapturePlaneDqCallback(
            struct v4l2_buffer *v4l2_buf,
            NvBuffer *buffer,
            NvBuffer *shared_buffer,
            void *arg);

    OutputStream* m_stream;
    UniqueObj<FrameConsumer> m_consumer;
    NvVideoEncoder *m_VideoEncoder;
    std::ofstream *m_outputFile;
    bool m_gotError;
    Values4CameraCtrl * pCamValues;
};
#endif // CONSUMERTHREAD_H
