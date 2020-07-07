cmake_minimum_required(VERSION 3.13)

project(Open3DStream)

add_subdirectory(thirdparty/nng nng)
add_subdirectory(thirdparty/flatbuffers flatbuffers)

add_library(open3dstream src/Open3DStream.cpp)
