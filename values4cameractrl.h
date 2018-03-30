#ifndef VALUES4CAMERACTRL_H
#define VALUES4CAMERACTRL_H


#include<libcams.h>
class Values4CameraCtrl
{
private:
    explicit Values4CameraCtrl();

public:
   static  Values4CameraCtrl* pInstance;
   static Values4CameraCtrl* getInstance();
    ~Values4CameraCtrl();
public:
        int MAX_ENCODER_FRAMES;
        int DEFAULT_FPS;
        int CAPTURE_TIME;

        Argus::Size2D<uint32_t> STREAM_SIZE;
        std::string  OUTPUT_FILENAME;
        uint32_t     ENCODER_PIXFMT;
        bool         DO_STAT;
        bool         VERBOSE_ENABLE;

};


#endif // VALUES4CAMERACTRL_H
