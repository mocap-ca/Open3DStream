SET(CMAKE_MIN_REQ_VERSION 3.13)
cmake_minimum_required(VERSION ${CMAKE_MIN_REQ_VERSION})

set(AppName Test1)
project(${AppName}Project)

find_package(nng CONFIG REQUIRED)

add_executable(${AppName} main.cpp)
target_link_libraries(${AppName} open3dstreamstatic  ${NNG_LIBRARY})

target_include_directories(${AppName} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/spdlog/include)

