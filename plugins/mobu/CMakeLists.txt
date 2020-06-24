cmake_minimum_required (VERSION 2.8.11)
project(StreamDevice)

set(SRCS device.cpp layout.cpp device.cpp device.h layout.h)

include_directories("C:/Program Files/Autodesk/MotionBuilder 2020/OpenRealitySDK/include")

link_directories("C:/Program Files/Autodesk/MotionBuilder 2020/OpenRealitySDK/lib/x64")

add_library(StreamDevice SHARED ${SRCS} )

target_link_libraries(StreamDevice fbsdk.lib opengl32.lib glu32.lib)

install(TARGETS StreamDevice DESTINATION "C:/Program Files/Autodesk/MotionBuilder 2020/bin/x64/plugins")