cmake_minimum_required(VERSION 2.8)
project(integration)

find_package(qibuild)
include_directories(../../include ../../include/kmlocal ../../lib32)

set(SRCDIR ../../src)

set(_srcs
    ${SRCDIR}/IplImageWrapper.cpp
    ${SRCDIR}/IplImagePyramid.cpp
    ${SRCDIR}/naodensetrack.cpp
    ${SRCDIR}/naokmeans.cpp
    ${SRCDIR}/naosvm.cpp
    ${SRCDIR}/integration.cpp
    ${SRCDIR}/naomngt.cpp
    )

qi_create_bin(integration ${_srcs} "main.cpp")
qi_use_lib(integration ALCOMMON OPENCV2_CORE OPENCV2_HIGHGUI OPENCV2_IMGPROC OPENCV2_VIDEO)
# Searching for shared libraries

target_link_libraries (integration libsvm.so libkmeans.so)
