#include "consumerthread.h"
#include<NvV4l2Element.h>
#include<NvV4l2ElementPlane.h>
ConsumerThread::ConsumerThread(OutputStream* stream) :
        m_stream(stream),
        m_VideoEncoder(NULL),
        m_outputFile(NULL),
        m_gotError(false)
{
    pCamValues = Values4CameraCtrl::getInstance ();
}

ConsumerThread::~ConsumerThread()
{
    if (m_VideoEncoder)
    {
        if (pCamValues->DO_STAT)
             m_VideoEncoder->printProfilingStats(std::cout);
        delete m_VideoEncoder;
    }

    if (m_outputFile)
        delete m_outputFile;
}

bool ConsumerThread::threadInitialize()
{
    // Create the FrameConsumer.
    m_consumer = UniqueObj<FrameConsumer>(FrameConsumer::create(m_stream));
    if (!m_consumer)
        ORIGINATE_ERROR("Failed to create FrameConsumer");

    // Create Video Encoder
    if (!createVideoEncoder())
        ORIGINATE_ERROR("Failed to create video m_VideoEncoderoder");

    // Create output file
    m_outputFile = new std::ofstream(pCamValues->OUTPUT_FILENAME.c_str());
    if (!m_outputFile)
        ORIGINATE_ERROR("Failed to open output file.");

    // Stream on
    int e = m_VideoEncoder->output_plane.setStreamStatus(true);
    if (e < 0)
        ORIGINATE_ERROR("Failed to stream on output plane");
    e = m_VideoEncoder->capture_plane.setStreamStatus(true);
    if (e < 0)
        ORIGINATE_ERROR("Failed to stream on capture plane");

    // Set video encoder callback
    m_VideoEncoder->capture_plane.setDQThreadCallback(encoderCapturePlaneDqCallback);

    // startDQThread starts a thread internally which calls the
    // encoderCapturePlaneDqCallback whenever a buffer is dequeued
    // on the plane
    m_VideoEncoder->capture_plane.startDQThread(this);

    // Enqueue all the empty capture plane buffers
    for (uint32_t i = 0; i < m_VideoEncoder->capture_plane.getNumBuffers(); i++)
    {
        struct v4l2_buffer v4l2_buf;
        struct v4l2_plane planes[MAX_PLANES];

        memset(&v4l2_buf, 0, sizeof(v4l2_buf));
        memset(planes, 0, MAX_PLANES * sizeof(struct v4l2_plane));

        v4l2_buf.index = i;
        v4l2_buf.m.planes = planes;

        CHECK_ERROR(m_VideoEncoder->capture_plane.qBuffer(v4l2_buf, NULL));
    }

    return true;
}

bool ConsumerThread::threadExecute()
{
    IStream *iStream = interface_cast<IStream>(m_stream);
    IFrameConsumer *iFrameConsumer = interface_cast<IFrameConsumer>(m_consumer);

    // Wait until the producer has connected to the stream.
    CONSUMER_PRINT("Waiting until producer is connected...\n");
    if (iStream->waitUntilConnected() != STATUS_OK)
        ORIGINATE_ERROR("Stream failed to connect.");
    CONSUMER_PRINT("Producer has connected; continuing.\n");

    int bufferIndex;

    bufferIndex = 0;

    // Keep acquire frames and queue into encoder
    while (!m_gotError)
    {
        clock_t tick_start = clock();
        NvBuffer *buffer;
        int fd = -1;

        struct v4l2_buffer v4l2_buf;
        struct v4l2_plane planes[MAX_PLANES];

        memset(&v4l2_buf, 0, sizeof(v4l2_buf));
        memset(planes, 0, MAX_PLANES * sizeof(struct v4l2_plane));

        v4l2_buf.m.planes = planes;

        // Check if we need dqBuffer first
        if (bufferIndex < pCamValues->MAX_ENCODER_FRAMES &&
            m_VideoEncoder->output_plane.getNumQueuedBuffers() <
            m_VideoEncoder->output_plane.getNumBuffers())
        {
            // The queue is not full, no need to dqBuffer
            // Prepare buffer index for the following qBuffer
            v4l2_buf.index = bufferIndex++;
        }
        else
        {
            // Output plane full or max outstanding number reached
            CHECK_ERROR(m_VideoEncoder->output_plane.dqBuffer(v4l2_buf, &buffer,
                                                              NULL, 10));
            // Release the frame.
            fd = v4l2_buf.m.planes[0].m.fd;
            NvBufferDestroy(fd);
            if (pCamValues->VERBOSE_ENABLE)
                CONSUMER_PRINT("Released frame. %d\n", fd);
        }

        // Acquire a frame.
        UniqueObj<Frame> frame(iFrameConsumer->acquireFrame());
        IFrame *iFrame = interface_cast<IFrame>(frame);
        if (!iFrame)
        {
            // Send EOS
            v4l2_buf.m.planes[0].m.fd = fd;
            v4l2_buf.m.planes[0].bytesused = 0;
            CHECK_ERROR(m_VideoEncoder->output_plane.qBuffer(v4l2_buf, NULL));
            break;
        }
 printf("Time duration( %f )ms from sample to queue buffer\n",double(clock()-tick_start)/CLOCKS_PER_SEC);
        // Get the IImageNativeBuffer extension interface and create the fd.
        NV::IImageNativeBuffer *iNativeBuffer =
            interface_cast<NV::IImageNativeBuffer>(iFrame->getImage());
        if (!iNativeBuffer)
            ORIGINATE_ERROR("IImageNativeBuffer not supported by Image.");
        fd = iNativeBuffer->createNvBuffer(pCamValues->STREAM_SIZE,
                                           NvBufferColorFormat_YUV420,
                                           NvBufferLayout_BlockLinear);
        if (pCamValues->VERBOSE_ENABLE)
            CONSUMER_PRINT("Acquired Frame. %d\n", fd);

        // Push the frame into V4L2.
        v4l2_buf.m.planes[0].m.fd = fd;
        v4l2_buf.m.planes[0].bytesused = 1; // byteused must be non-zero
        CHECK_ERROR(m_VideoEncoder->output_plane.qBuffer(v4l2_buf, NULL));

    }

    // Wait till capture plane DQ Thread finishes
    // i.e. all the capture plane buffers are dequeued
    m_VideoEncoder->capture_plane.waitForDQThread(2000);

    CONSUMER_PRINT("Done.\n");

    requestShutdown();

    return true;
}

bool ConsumerThread::threadShutdown()
{
    return true;
}

bool ConsumerThread::createVideoEncoder()
{
    int ret = 0;

    m_VideoEncoder = NvVideoEncoder::createVideoEncoder("enc0");
    if (!m_VideoEncoder)
        ORIGINATE_ERROR("Could not create m_VideoEncoderoder");

    if (pCamValues->DO_STAT)
        m_VideoEncoder->enableProfiling();

    ret = m_VideoEncoder->setCapturePlaneFormat(pCamValues->ENCODER_PIXFMT, pCamValues->STREAM_SIZE.width(),
                                    pCamValues->STREAM_SIZE.height(), 2 * 1024 * 1024);
    if (ret < 0)
        ORIGINATE_ERROR("Could not set capture plane format");

    ret = m_VideoEncoder->setOutputPlaneFormat(V4L2_PIX_FMT_YUV420M,pCamValues-> STREAM_SIZE.width(),
                                    pCamValues->STREAM_SIZE.height());
    if (ret < 0)
        ORIGINATE_ERROR("Could not set output plane format");

    ret = m_VideoEncoder->setBitrate(4 * 1024 * 1024);
    if (ret < 0)
        ORIGINATE_ERROR("Could not set bitrate");

    if (pCamValues->ENCODER_PIXFMT == V4L2_PIX_FMT_H264)
    {
        ret = m_VideoEncoder->setProfile(V4L2_MPEG_VIDEO_H264_PROFILE_HIGH);
    }
    else
    {
        ret = m_VideoEncoder->setProfile(V4L2_MPEG_VIDEO_H265_PROFILE_MAIN);
    }
    if (ret < 0)
        ORIGINATE_ERROR("Could not set m_VideoEncoderoder profile");

    if (pCamValues->ENCODER_PIXFMT == V4L2_PIX_FMT_H264)
    {
        ret = m_VideoEncoder->setLevel(V4L2_MPEG_VIDEO_H264_LEVEL_5_0);
        if (ret < 0)
            ORIGINATE_ERROR("Could not set m_VideoEncoderoder level");
    }

    ret = m_VideoEncoder->setRateControlMode(V4L2_MPEG_VIDEO_BITRATE_MODE_CBR);
    if (ret < 0)
        ORIGINATE_ERROR("Could not set rate control mode");

    ret = m_VideoEncoder->setIFrameInterval(30);
    if (ret < 0)
        ORIGINATE_ERROR("Could not set I-frame interval");

    ret = m_VideoEncoder->setFrameRate(30, 1);
    if (ret < 0)
        ORIGINATE_ERROR("Could not set m_VideoEncoderoder framerate");

    ret = m_VideoEncoder->setHWPresetType(V4L2_ENC_HW_PRESET_ULTRAFAST);
    if (ret < 0)
        ORIGINATE_ERROR("Could not set m_VideoEncoderoder HW Preset");

    // Query, Export and Map the output plane buffers so that we can read
    // raw data into the buffers
    ret = m_VideoEncoder->output_plane.setupPlane(V4L2_MEMORY_DMABUF, 10, true, false);
    if (ret < 0)
        ORIGINATE_ERROR("Could not setup output plane");

    // Query, Export and Map the output plane buffers so that we can write
    // m_VideoEncoderoded data from the buffers
    ret = m_VideoEncoder->capture_plane.setupPlane(V4L2_MEMORY_MMAP, 10, true, false);
    if (ret < 0)
        ORIGINATE_ERROR("Could not setup capture plane");

    printf("create video encoder return true\n");
    return true;
}

void ConsumerThread::abort()
{
    m_VideoEncoder->abort();
    m_gotError = true;
}

bool ConsumerThread::encoderCapturePlaneDqCallback(struct v4l2_buffer *v4l2_buf,
                                                   NvBuffer * buffer,
                                                   NvBuffer * shared_buffer,
                                                   void *arg)
{
    ConsumerThread *thiz = (ConsumerThread*)arg;

    if (!v4l2_buf)
    {
        thiz->abort();
        ORIGINATE_ERROR("Failed to dequeue buffer from encoder capture plane");
    }

    thiz->m_outputFile->write((char *) buffer->planes[0].data,
                              buffer->planes[0].bytesused);

    if (thiz->m_VideoEncoder->capture_plane.qBuffer(*v4l2_buf, NULL) < 0)
    {
        thiz->abort();
        ORIGINATE_ERROR("Failed to enqueue buffer to encoder capture plane");
        return false;
    }

    // GOT EOS from m_VideoEncoderoder. Stop dqthread.
    if (buffer->planes[0].bytesused == 0)
    {
        CONSUMER_PRINT("Got EOS, exiting...\n");
        return false;
    }

    return true;
}

