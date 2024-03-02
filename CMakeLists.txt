cmake_minimum_required(VERSION  3.13)

project(Open3DStream VERSION 0.1.0)

#if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
#  set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_LIST_DIR}/usr )
#endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_DEBUG_POSTFIX d)

#add_subdirectory(apps/FbxStream)
add_subdirectory(src)     
add_subdirectory(apps/Repeater)
add_subdirectory(apps/SubscribeTest)
add_subdirectory(plugins/mobu)
#add_subdirectory(plugins/maya)

