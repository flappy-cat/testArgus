#include "values4cameractrl.h"


Values4CameraCtrl::Values4CameraCtrl()
{
    // Constant configuration.
    MAX_ENCODER_FRAMES = 5;
    DEFAULT_FPS        = 30;

    // Configurations which can be overrided by cmdline
   CAPTURE_TIME = 10; // In seconds.
    STREAM_SIZE = Size2D<uint32_t> (1280, 720);
   OUTPUT_FILENAME ="output.h264";
    ENCODER_PIXFMT = V4L2_PIX_FMT_H264;
    DO_STAT = false;
   VERBOSE_ENABLE = false;
}

Values4CameraCtrl* Values4CameraCtrl::pInstance = NULL;
Values4CameraCtrl* Values4CameraCtrl::getInstance ()
{
    if(NULL == pInstance)
    {
        pInstance = new Values4CameraCtrl();

    }
    return pInstance;
}
Values4CameraCtrl::~Values4CameraCtrl ()
{
    if(pInstance != NULL)
    {
        delete pInstance;
    }
    pInstance = NULL;
}
