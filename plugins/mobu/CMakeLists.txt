cmake_minimum_required (VERSION 2.8.11)
project(MobuStreamDevice)

set(SRCS plugin.cpp layout.cpp device.cpp device.h layout.h mobuModel.cpp mobuModel.cpp)

include_directories("C:/Program Files/Autodesk/MotionBuilder 2020/OpenRealitySDK/include")

link_directories("C:/Program Files/Autodesk/MotionBuilder 2020/OpenRealitySDK/lib/x64")

add_library(O3DSMobuDevice SHARED ${SRCS} )

target_link_libraries(O3DSMobuDevice  fbsdk.lib opengl32.lib glu32.lib Ws2_32.lib open3dstreamstatic ${libs})

install(TARGETS O3DSMobuDevice DESTINATION "C:/Program Files/Autodesk/MotionBuilder 2020/bin/x64/plugins")