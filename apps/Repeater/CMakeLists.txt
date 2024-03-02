SET(CMAKE_MIN_REQ_VERSION 3.13)
cmake_minimum_required(VERSION ${CMAKE_MIN_REQ_VERSION})

project(Repeater)

find_package(nng CONFIG REQUIRED)

add_executable(Repeater main.cpp)
target_link_libraries(Repeater open3dstreamstatic ${NNG_LIBRARY})

install(TARGETS Repeater DESTINATION bin)

if(WIN32)
if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
add_custom_command(TARGET Repeater POST_BUILD       
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  
        ${fbxroot}/lib/vs2017/x64/$<CONFIGURATION>/libfbxsdk.dll     
        $<TARGET_FILE_DIR:Repeater>)   
endif()
endif()


