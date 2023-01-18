cmake_minimum_required(VERSION  3.13)

project(Open3DStream VERSION 0.1.0)

add_definitions(
    -DO3DS_VERSION=${PROJECT_VERSION}
    -DO3DS_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    -DO3DS_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    -DO3DS_VERSION_PATCH=${PROJECT_VERSION_PATCH} )

#if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
#  set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_LIST_DIR}/usr )
#endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_DEBUG_POSTFIX d)

find_package(flatbuffers CONFIG REQUIRED flatbuffers flatc)
find_package(nng CONFIG REQUIRED)

include_directories( ${FLATBUFFERS_INCLUDE_DIRS} ${NNG_INCLUDE_DIRS}/..)

add_subdirectory(apps/FbxStream)
add_subdirectory(apps/Repeater)
#add_subdirectory(apps/SubscribeTest)
add_subdirectory(src)     
add_subdirectory(plugins/mobu)
add_subdirectory(plugins/maya)

