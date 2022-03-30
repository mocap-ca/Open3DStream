cmake_minimum_required(VERSION  3.13)

project(Open3DStream)

# BOOST

if(UNIX)
find_package(Boost COMPONENTS thread)
else()
find_package(Boost)
endif()

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find boost - set BOOST_ROOT?")
endif()

set(libs debug flatbuffersd optimized flatbuffers debug nngd optimized nng ${Boost_LIBRARIES})

set(CMAKE_CXX_STANDARD 17)

include_directories( 
    ${Boost_INCLUDE_DIRS} 
    ${CMAKE_CURRENT_LIST_DIR}/include 
    ${CMAKE_CURRENT_LIST_DIR}/include/websocketpp
    ${CMAKE_CURRENT_LIST_DIR}/src)

link_directories(${Boost_LIBRARY_DIRS} ${CMAKE_CURRENT_LIST_DIR}/lib)
if (UNIX)
	link_directories(${CMAKE_CURRENT_LIST_DIR}/lib64)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -pthread -ldl")
    # -std=c++14
endif()

# Linking nng static lib, so need to define this otherwise we will get linker errors.
# see - thirdparty\nng\include\nng\nng.h:32
add_definitions(-DNNG_STATIC_LIB )

if(WIN32)

	# As we are adding full paths to the debug/release fbx libs, we need to set the CMP0003 policy
	if(COMMAND cmake_policy)
		cmake_policy(SET CMP0003 NEW)
	endif(COMMAND cmake_policy)

	list(APPEND libs wsock32 ws2_32)
	set(MSVC_VERSION "1911")
endif()


set(CMAKE_DEBUG_POSTFIX d)

# Build the flatbuffer schema
set(schema_in ${CMAKE_CURRENT_LIST_DIR}/protocol/schema.fbs) 
set(schema_out ${CMAKE_CURRENT_LIST_DIR}/include/schema_generated.h)

add_custom_command(
    OUTPUT ${schema_out}
    COMMAND ${CMAKE_CURRENT_LIST_DIR}/bin/flatc -o ${CMAKE_CURRENT_LIST_DIR}/include --cpp ${schema_in} 
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/protocol/schema.fbs
)

add_custom_target(protocol DEPENDS ${schema_out})
        
add_subdirectory(apps/FbxStream)
add_subdirectory(apps/Repeater)
add_subdirectory(apps/SubscribeTest)
add_subdirectory(src)     
add_subdirectory(plugins/mobu)
add_subdirectory(plugins/maya)

