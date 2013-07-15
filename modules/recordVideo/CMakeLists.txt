cmake_minimum_required(VERSION 2.8)
project(recordVideo)

find_package(qibuild)

# Create a executable named recordVideo
# with the source file: main.cpp
qi_create_bin(recordVideo "main.cpp")
qi_use_lib(recordVideo ALCOMMON)
