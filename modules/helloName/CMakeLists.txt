cmake_minimum_required(VERSION 2.8)
project(helloName)

find_package(qibuild)

# Create a executable named helloName
# with the source file: main.cpp
qi_create_bin(helloName "main.cpp")
qi_use_lib(helloName ALCOMMON)
