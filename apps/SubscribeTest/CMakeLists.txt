SET(CMAKE_MIN_REQ_VERSION 3.13)
cmake_minimum_required(VERSION ${CMAKE_MIN_REQ_VERSION})

project(SubscribeTest)

find_package(nng CONFIG REQUIRED)

add_executable(SubscribeTest main.cpp)


#set(libs debug flatbuffersd optimized flatbuffers debug nngd optimized nng)
#if(WIN32)
#	set(libs wsock32 ws2_32 ${libs})
#endif()

target_link_libraries(SubscribeTest PRIVATE open3dstreamstatic ${libs} ${NNG_LIBRARY})

set_target_properties(SubscribeTest PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "ws ws://rt.zip.media:80")
