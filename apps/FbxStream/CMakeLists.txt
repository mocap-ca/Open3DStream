SET(CMAKE_MIN_REQ_VERSION 2.4)
cmake_minimum_required(VERSION ${CMAKE_MIN_REQ_VERSION})

project(FbxStream)

set(fbxroot /home/ec2-user/fbxsdk)

if (UNIX)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -ldl")
	include_directories(${fbxroot}/include
		${CMAKE_CURRENT_LIST_DIR}/../../src
		${CMAKE_CURRENT_LIST_DIR}/../../include
		${CMAKE_CURRENT_LIST_DIR}/../../protocol
		)
	link_directories(${fbxroot}/lib/gcc4/x64/release/)
	set(libs flatbuffers nng fbxsdk)
endif()

if(WIN32)
link_directories(${CMAKE_CURRENT_LIST_DIR}/../../lib)
include("C:/Program Files/Autodesk/FBX/FBX SDK/2020.0.1/samples/CMakeSettings.txt")
add_definitions(-DNNG_STATIC_LIB )
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../include)
set(libs wsock32 ws2_32 debug flatbuffersd optimized flatbuffers debug nngd optimized nng)
set(MSVC_VERSION "1911")
include_directories(
	"${CMAKE_CURRENT_LIST_DIR}/../../src"
	"${CMAKE_CURRENT_LIST_DIR}/../../include"
	"${CMAKE_CURRENT_LIST_DIR}/../../thirdparty/cml"
	)
endif()

#FBX_PROJECT(FbxTest .)

add_executable(
	FbxStream
   main.cpp fbxloader.cpp fbxloader.h
   ${CMAKE_CURRENT_LIST_DIR}/../../src/o3ds/broadcaster.h
   ${CMAKE_CURRENT_LIST_DIR}/../../src/o3ds/broadcaster.cpp
   ${CMAKE_CURRENT_LIST_DIR}/../../src/get_time.h
   ${CMAKE_CURRENT_LIST_DIR}/../../src/get_time.cpp
   ${CMAKE_CURRENT_LIST_DIR}/../../src/Open3dStreamModel.h
   ${CMAKE_CURRENT_LIST_DIR}/../../src/Open3dStreamModel.cpp   
)

target_link_libraries(FbxStream PRIVATE ${libs} )

