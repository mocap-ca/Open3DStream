SET(CMAKE_MIN_REQ_VERSION 3.13)
cmake_minimum_required(VERSION ${CMAKE_MIN_REQ_VERSION})

project(Repeater)

add_executable(Repeater main.cpp 
	${CMAKE_CURRENT_LIST_DIR}/../../src/o3ds/websocket.cpp
	${CMAKE_CURRENT_LIST_DIR}/../../src/o3ds/nng_connector.cpp
	${CMAKE_CURRENT_LIST_DIR}/../../src/o3ds/base_connector.cpp
	${CMAKE_CURRENT_LIST_DIR}/../../src/o3ds/pair.cpp
	${CMAKE_CURRENT_LIST_DIR}/../../src/o3ds/publisher.cpp
)

if(WIN32)
if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
add_custom_command(TARGET Repeater POST_BUILD       
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  
        ${fbxroot}/lib/vs2017/x64/$<CONFIGURATION>/libfbxsdk.dll     
        $<TARGET_FILE_DIR:Repeater>)   
endif()
endif()


