cmake_minimum_required(VERSION 2.8)
project(densetrack)

find_package(qibuild)

set(_srcs
    IplImageWrapper.h
    IplImageWrapper.cpp
    IplImagePyramid.h
    IplImagePyramid.cpp
    Box.h
    denseTrack.h
    functions.h
    numericFunctions.h
    Point.h
    Size.h
)

qi_create_bin(densetrack ${_srcs} "main.cpp")
qi_use_lib(densetrack ALCOMMON OPENCV2_CORE OPENCV2_HIGHGUI OPENCV2_IMGPROC OPENCV2_VIDEO)
