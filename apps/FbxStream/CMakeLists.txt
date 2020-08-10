SET(CMAKE_MIN_REQ_VERSION 2.4)
cmake_minimum_required(VERSION ${CMAKE_MIN_REQ_VERSION})

set(MSVC_VERSION "1911")

include("C:/Program Files/Autodesk/FBX/FBX SDK/2020.0.1/samples/CMakeSettings.txt")


include_directories(
    "${CMAKE_CURRENT_LIST_DIR}/../../src"
	"${CMAKE_CURRENT_LIST_DIR}/../../protocol"
	 "${CMAKE_CURRENT_LIST_DIR}/../../thirdparty/flatbuffers/include"
	 "${CMAKE_CURRENT_LIST_DIR}/../../thirdparty/cml"
	)

FBX_PROJECT(FbxTest .)

SET(FBX_TARGET_SOURCE main.cpp
   ${CMAKE_CURRENT_LIST_DIR}/../../src/get_time.h
   ${CMAKE_CURRENT_LIST_DIR}/../../src/get_time.cpp
   ${CMAKE_CURRENT_LIST_DIR}/../../src/Open3dStreamModel.h
   ${CMAKE_CURRENT_LIST_DIR}/../../src/Open3dStreamModel.cpp
   
   )

ADD_EXECUTABLE(
   ${FBX_TARGET_NAME}
   ${FBX_TARGET_SOURCE}
)
SET_SAMPLES_GLOBAL_FLAGS()
