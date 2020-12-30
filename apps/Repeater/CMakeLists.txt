SET(CMAKE_MIN_REQ_VERSION 3.13)
cmake_minimum_required(VERSION ${CMAKE_MIN_REQ_VERSION})

project(Repeater)

set(CMAKE_DEBUG_POSTFIX d)

set(libs debug flatbuffersd optimized flatbuffers debug nngd optimized nng)

if (UNIX)
	if(CMAKE_BUILD_TYPE MATCHES Release)
	link_directories(${fbxroot}/lib/gcc4/x64/release/)
	endif()
	if(CMAKE_BUILD_TYPE MATCHES Debug)
	link_directories(${fbxroot}/lib/gcc4/x64/debug/)
	endif()
endif()

if(WIN32)
	link_directories(${CMAKE_CURRENT_LIST_DIR}/../../lib)
	add_definitions(-DNNG_STATIC_LIB )

	# As we are adding full paths to the debug/release fbx libs, we need to set the CMP0003 policy
	if(COMMAND cmake_policy)
		cmake_policy(SET CMP0003 NEW)
	endif(COMMAND cmake_policy)

	set(libs ${libs} wsock32 ws2_32)
	set(MSVC_VERSION "1911")
endif()

add_executable(Repeater main.cpp 
	${CMAKE_CURRENT_LIST_DIR}/../../src/o3ds/base_server.cpp
	${CMAKE_CURRENT_LIST_DIR}/../../src/o3ds/pair.cpp
	${CMAKE_CURRENT_LIST_DIR}/../../src/o3ds/publisher.cpp
)

target_link_libraries(Repeater PRIVATE ${libs} )

target_include_directories(Repeater PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../src ${CMAKE_CURRENT_LIST_DIR}/../../include)

if(WIN32)
if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
add_custom_command(TARGET Repeater POST_BUILD       
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  
        ${fbxroot}/lib/vs2017/x64/$<CONFIGURATION>/libfbxsdk.dll     
        $<TARGET_FILE_DIR:Repeater>)   
endif()
endif()


