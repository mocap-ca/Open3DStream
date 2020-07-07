cmake_minimum_required(VERSION 3.13)

project(Open3DStream)

add_subdirectory(thirdparty/nng ${CMAKE_CURRENT_BINARY_DIR}/nng)

# Add FlatBuffers directly to our build. This defines the `flatbuffers` target.
add_subdirectory(thirdparty/flatbuffers
                 ${CMAKE_CURRENT_BINARY_DIR}/flatbuffers
                 EXCLUDE_FROM_ALL)

include_directories(thirdparty/nng/include
                    thirdparty/flatbuffers/include)

add_library(open3dstream SHARED src/Open3DStream.cpp)
add_library(open3dstreamstatic STATIC src/Open3DStream.cpp)

# TODO: Put libs in a variable and pass to both - dv
target_link_libraries(open3dstream PRIVATE flatbuffers
                                           nng)

target_link_libraries(open3dstreamstatic PRIVATE flatbuffers
                                                 nng)
