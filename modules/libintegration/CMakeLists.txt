cmake_minimum_required(VERSION 2.8)
project(libintegration)

find_package(qibuild)

# Root directory
set(ROOTDIR ../..)

# Source code, include and libraries directories
set(SRCDIR ${ROOTDIR}/src)
set(INCLUDEDIR ${ROOTDIR}/include)
# set(LIBDIR ${ROOTDIR}/lib32)

include_directories(${INCLUDEDIR}/kmlocal ${INCLUDEDIR} /usr/local/include)

set(_srcs
    ${SRCDIR}/IplImageWrapper.cpp
    ${SRCDIR}/IplImagePyramid.cpp
    ${SRCDIR}/naodensetrack.cpp
    ${SRCDIR}/naokmeans.cpp
    ${SRCDIR}/naosvm.cpp
    ${SRCDIR}/integration.cpp
    ${SRCDIR}/naomngt.cpp
    ${SRCDIR}/imconfig.cpp
    tactil.cpp
    tactil.h
    )

qi_create_lib(libintegration SHARED ${_srcs} "main.cpp")
qi_use_lib(libintegration ALCOMMON OPENCV2_CORE OPENCV2_HIGHGUI OPENCV2_IMGPROC OPENCV2_VIDEO)
# Searching for shared libraries

target_link_libraries (libintegration libsvm.so libkmeans.so libftp.so)
