#ifndef LIBCAMS_H
#define LIBCAMS_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "Error.h"
#include "Thread.h"

#include <Argus/Argus.h>
#include <EGLStream/EGLStream.h>
#include <EGLStream/NV/ImageNativeBuffer.h>

#include<NvApplicationProfiler.h>
#include<linux/videodev2.h>


#include<NvBuffer.h>
#include<NvVideoEncoder.h>
#include<values4cameractrl.h>
using namespace std;
using namespace Argus;
using namespace EGLStream;
using namespace ArgusSamples;
// Debug print macros.
#define PRODUCER_PRINT(...) printf("PRODUCER: " __VA_ARGS__)
#define CONSUMER_PRINT(...) printf("CONSUMER: " __VA_ARGS__)
#define CHECK_ERROR(expr) \
    do { \
        if ((expr) < 0) { \
            abort(); \
            ORIGINATE_ERROR(#expr " failed"); \
        } \
    } while (0);
#endif // LIBCAMS_H

