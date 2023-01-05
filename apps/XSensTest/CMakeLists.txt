cmake_minimum_required(VERSION  3.13)

project(xsens_test)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(XSensTest main.cpp)

target_link_libraries(XSensTest open3dstreamstatic )