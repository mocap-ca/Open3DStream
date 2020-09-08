SET(CMAKE_MIN_REQ_VERSION 3.13)
cmake_minimum_required(VERSION ${CMAKE_MIN_REQ_VERSION})

project(FbxStream)


if (UNIX)
	set(fbxroot /home/ec2-user/fbxsdk)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -ldl")
	link_directories(${fbxroot}/lib/gcc4/x64/release/)
	set(libs flatbuffers nng fbxsdk)
endif()

if(WIN32)
	link_directories(${CMAKE_CURRENT_LIST_DIR}/../../lib)
	set(fbxroot "C:/Program Files/Autodesk/FBX/FBX SDK/2020.0.1") 
	include_directories("${fbxroot}/include")
	add_definitions(-DNNG_STATIC_LIB )

	# This will only work with newer versions of visual studio (untested)
	# https://cmake.org/cmake/help/git-stage/prop_tgt/VS_DEBUGGER_COMMAND_ARGUMENTS.html
	set(VS_DEBUGGER_COMMAND_ARGUMENTS ${CMAKE_CURRENT_LIST_DIR}../../test_data/beta_fight.fbx tcp://127.0.0.1:7000)

	# As we are adding full paths to the debug/release fbx libs, we need to set the CMP0003 policy
	if(COMMAND cmake_policy)
		cmake_policy(SET CMP0003 NEW)
	endif(COMMAND cmake_policy)

	set(libs wsock32 ws2_32 debug flatbuffersd optimized flatbuffers debug nngd optimized nng
		debug ${fbxroot}/lib/vs2017/x64/debug/libfbxsdk.lib  
		optimized ${fbxroot}/lib/vs2017/x64/release/libfbxsdk.lib 
	)
	set(MSVC_VERSION "1911")
endif()

add_executable(FbxStream main.cpp fbxloader.cpp fbxloader.h)

target_link_libraries(FbxStream PRIVATE ${libs} open3dstreamstatic)

if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
add_custom_command(TARGET FbxStream POST_BUILD       
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  
        ${fbxroot}/lib/vs2017/x64/$<CONFIGURATION>/libfbxsdk.dll     
        $<TARGET_FILE_DIR:FbxStream>)   
endif()