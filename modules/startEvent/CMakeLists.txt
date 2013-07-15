cmake_minimum_required(VERSION 2.8)
project(startevent)

find_package(qibuild)

qi_create_bin(startevent "main.cpp")
qi_use_lib(startevent ALCOMMON)
