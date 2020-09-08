SET(CMAKE_MIN_REQ_VERSION 3.13)
cmake_minimum_required(VERSION ${CMAKE_MIN_REQ_VERSION})

project(SubscribeTest)

add_executable(SubscribeTest main.cpp)

set(libs wsock32 ws2_32 debug flatbuffersd optimized flatbuffers debug nngd optimized nng)

target_link_libraries(SubscribeTest PRIVATE open3dstreamstatic ${libs})
