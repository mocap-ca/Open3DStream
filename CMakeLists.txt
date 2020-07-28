cmake_minimum_required(VERSION 3.13)

project(Open3DStream)

add_subdirectory(thirdparty/nng ${CMAKE_CURRENT_BINARY_DIR}/nng)

# Add FlatBuffers directly to our build. This defines the `flatbuffers` target.
add_subdirectory(thirdparty/flatbuffers
                 ${CMAKE_CURRENT_BINARY_DIR}/flatbuffers
)

include_directories(thirdparty/nng/include
                    thirdparty/flatbuffers/include)

# Shared library
add_library(open3dstream SHARED src/Open3DStreamServer.cpp
                                src/Open3DStreamClient.cpp)

# Static linkable library
add_library(open3dstreamstatic STATIC src/Open3DStreamServer.cpp
                                      src/Open3DStreamClient.cpp)

# Command line tools.
add_executable(o3ds_pingServer src/cmd/o3ds_pingServer.c)
add_executable(o3ds_ping src/cmd/o3ds_ping.c)

target_link_libraries(open3dstream PRIVATE flatbuffers
                                           nng)

target_link_libraries(open3dstreamstatic PRIVATE flatbuffers
                                                 nng)

target_link_libraries(o3ds_pingServer PRIVATE flatbuffers nng)
target_link_libraries(o3ds_ping PRIVATE flatbuffers nng)
