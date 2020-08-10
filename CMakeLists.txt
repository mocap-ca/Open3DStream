cmake_minimum_required(VERSION 3.13)

project(Open3DStream)

set(CMAKE_DEBUG_POSTFIX d)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_definitions(-DNNG_STATIC_LIB )

set(libs wsock32 ws2_32 debug flatbuffersd optimized flatbuffers debug nngd optimized nng)

#add_subdirectory(thirdparty/nng ${CMAKE_CURRENT_BINARY_DIR}/nng)

# Add FlatBuffers directly to our build. This defines the `flatbuffers` target.
#add_subdirectory(thirdparty/flatbuffers
#                 ${CMAKE_CURRENT_BINARY_DIR}/flatbuffers
#)

#include_directories(thirdparty/nng/include
#                    thirdparty/flatbuffers/include)

# Shared library
add_library(open3dstream SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/Open3dStreamServer.cpp
                                ${CMAKE_CURRENT_SOURCE_DIR}/src/Open3dStreamClient.cpp)

# Static linkable library
add_library(open3dstreamstatic STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/Open3dStreamServer.cpp
                                      ${CMAKE_CURRENT_SOURCE_DIR}/src/Open3dStreamClient.cpp)
									  
									  

# Command line tools.
add_executable(o3ds_pingServer ${CMAKE_CURRENT_SOURCE_DIR}/src/cmd/o3ds_pingServer.c)
add_executable(o3ds_ping ${CMAKE_CURRENT_SOURCE_DIR}/src/cmd/o3ds_ping.c)


target_link_libraries(open3dstream PRIVATE ${libs} )

target_link_libraries(open3dstreamstatic PRIVATE ${libs})

target_link_libraries(o3ds_pingServer PRIVATE ${libs})
										   
target_link_libraries(o3ds_ping PRIVATE ${libs})
