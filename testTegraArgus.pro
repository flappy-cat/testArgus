QT += core
QT -= gui

TARGET = testTegraArgus
CONFIG += console c++11
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    consumerthread.cpp \
    ../../tegra_multimedia_api/samples/common/classes/NvVideoEncoder.cpp \
    values4cameractrl.cpp \
    ../../tegra_multimedia_api/argus/samples/utils/Thread.cpp \
    ../../tegra_multimedia_api/samples/common/classes/NvV4l2ElementPlane.cpp \
    ../../tegra_multimedia_api/samples/common/classes/NvV4l2Element.cpp \
    ../../tegra_multimedia_api/samples/common/classes/NvApplicationProfiler.cpp \
    ../../tegra_multimedia_api/samples/common/classes/NvBuffer.cpp \
    ../../tegra_multimedia_api/samples/common/classes/NvDrmRenderer.cpp \
    ../../tegra_multimedia_api/samples/common/classes/NvElement.cpp \
    ../../tegra_multimedia_api/samples/common/classes/NvElementProfiler.cpp \
    ../../tegra_multimedia_api/samples/common/classes/NvLogging.cpp \
    ../../tegra_multimedia_api/samples/common/classes/NvUtils.cpp \
    ../../tegra_multimedia_api/samples/common/classes/NvVideoConverter.cpp \
    ../../tegra_multimedia_api/samples/common/classes/NvVideoDecoder.cpp

INCLUDEPATH += /usr/include/libdrm/
INCLUDEPATH += /home/nvidia/tegra_multimedia_api/include
INCLUDEPATH += /home/nvidia/tegra_multimedia_api/argus/include /home/nvidia/tegra_multimedia_api/argus/include/Argus
INCLUDEPATH += /home/nvidia/tegra_multimedia_api/argus/include /home/nvidia/tegra_multimedia_api/argus/include/EGLStream
INCLUDEPATH +=/home/nvidia/tegra_multimedia_api/argus/samples/utils


LIBS+= -L/usr/lib/aarch64-linux-gnu/ -lv4l2
LIBS+= -L/usr/lib/aarch64-linux-gnu/tegra/  \
    -largus -largus_socketclient    \
    -largus_socketserver  -lnvcamlog -lnvcolorutil -lnvdc  \
    -lEGL -lGL -lGLU -lnvbuf_utils -ldrm -lcuda -lglx -lnvcamerautils -lnvapputil -lnvcameratools -ltegrav4l2 -lnvll -lnvmedia -lnvos -lnvmm_utils -lnvcam_imageencoder -lnveglstream_camconsumer -lnvavp
LIBS+= -L/usr/lib/aarch64-linux-gnu/tegra-egl/ -lEGL
#LIBS+= /usr/lib/aarch64-linux-gnu/libv4l/v4l2convert.so
LIBS+= -L/usr/lib/aarch64-linux-gnu/libv4l/plugins/ -lv4l2_nvvidconv -lv4l2_nvvideocodec -lv4l-mplane
HEADERS += \
    consumerthread.h \
    libcams.h \
    ../../tegra_multimedia_api/include/NvVideoEncoder.h \
    values4cameractrl.h \
    ../../tegra_multimedia_api/include/nvbuf_utils.h \
    ../../tegra_multimedia_api/include/NvBuffer.h

