cmake_minimum_required(VERSION  3.13)
project(O3DS3rdParty)

set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_LIST_DIR}/..)

add_subdirectory(flatbuffers)
add_subdirectory(nng)
add_subdirectory(websocketpp)