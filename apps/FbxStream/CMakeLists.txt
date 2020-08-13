SET(CMAKE_MIN_REQ_VERSION 2.4)
cmake_minimum_required(VERSION ${CMAKE_MIN_REQ_VERSION})

project(FbxStream)

set(MSVC_VERSION "1911")

include_directories(${CMAKE_CURRENT_LIST_DIR}/../../include)
link_directories(${CMAKE_CURRENT_LIST_DIR}/../../lib)

add_definitions(-DNNG_STATIC_LIB )

set(libs wsock32 ws2_32 debug flatbuffersd optimized flatbuffers debug nngd optimized nng)

include("C:/Program Files/Autodesk/FBX/FBX SDK/2020.0.1/samples/CMakeSettings.txt")

SET(FBX_PROJECT_NAME   FbxStream)
SET(FBX_TARGET_NAME    FbxStream)
SET(FBX_PROJECT_SUBDIR "." )
SET(PROJECT_BINARY_DIR     "${CMAKE_CURRENT_LIST_DIR}")
SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}")
SET(LIBRARY_OUTPUT_PATH    "${CMAKE_CURRENT_LIST_DIR}")


include_directories(
    "${CMAKE_CURRENT_LIST_DIR}/../../src"
	"${CMAKE_CURRENT_LIST_DIR}/../../include"
	 "${CMAKE_CURRENT_LIST_DIR}/../../thirdparty/cml"
	)

#FBX_PROJECT(FbxTest .)

SET(FBX_TARGET_SOURCE main.cpp fbxloader.cpp fbxloader.h
   ${CMAKE_CURRENT_LIST_DIR}/../../src/o3ds/broadcaster.h
   ${CMAKE_CURRENT_LIST_DIR}/../../src/o3ds/broadcaster.cpp
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

target_link_libraries(FbxStream PRIVATE ${libs} )

