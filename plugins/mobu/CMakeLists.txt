cmake_minimum_required (VERSION 2.8.11)
project(MobuStreamDevice)

add_definitions( -DO3DS_MOBU_PLUIN_VER="0.2")

set(SRCS plugin.cpp layout.cpp device.cpp device.h layout.h mobuModel.h mobuModel.cpp)

if(EXISTS  "C:/Program Files/Autodesk/MotionBuilder 2019/")
add_library(O3DSMobu2019Device  SHARED ${SRCS} )
target_link_libraries(O3DSMobu2019Device  fbsdk.lib opengl32.lib glu32.lib Ws2_32.lib open3dstreamstatic ${libs})
target_include_directories(O3DSMobu2019Device PUBLIC "C:/Program Files/Autodesk/MotionBuilder 2019/OpenRealitySDK/include")
target_link_directories(O3DSMobu2019Device PUBLIC "C:/Program Files/Autodesk/MotionBuilder 2019/OpenRealitySDK/lib/x64")
install(TARGETS O3DSMobu2019Device DESTINATION "C:/Program Files/Autodesk/MotionBuilder 2019/bin/x64/plugins")
endif()

if(EXISTS  "C:/Program Files/Autodesk/MotionBuilder 2020/")
add_library(O3DSMobu2020Device  SHARED ${SRCS} )
target_link_libraries(O3DSMobu2020Device  fbsdk.lib opengl32.lib glu32.lib Ws2_32.lib open3dstreamstatic ${libs})
target_include_directories(O3DSMobu2020Device PUBLIC "C:/Program Files/Autodesk/MotionBuilder 2020/OpenRealitySDK/include")
target_link_directories(O3DSMobu2020Device PUBLIC  "C:/Program Files/Autodesk/MotionBuilder 2020/OpenRealitySDK/lib/x64")
install(TARGETS O3DSMobu2020Device DESTINATION "C:/Program Files/Autodesk/MotionBuilder 2020/bin/x64/plugins")
endif()


