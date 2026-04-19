SET(CMAKE_MIN_REQ_VERSION 3.13)
cmake_minimum_required(VERSION ${CMAKE_MIN_REQ_VERSION})

set(AppName Test2)
project(${AppName}Project)

find_package(nng CONFIG REQUIRED)
find_package(flatbuffers CONFIG REQUIRED HINTS ${CMAKE_CURRENT_LIST_DIR}/../../usr) 

add_executable(${AppName} 
	main.cpp 
	test_dynamic_hierarchy.cpp
	../../src/o3ds/model.cpp 
	../../src/o3ds/o3ds.cpp 
	../../src/o3ds/getTime.cpp
	../../src/o3ds/o3ds_version.cpp	
	../../src/o3ds/math.cpp	
	../../src/o3ds/math.h	
	../../src/o3ds/context.cpp	
)

set_target_properties(${AppName} PROPERTIES DEBUG_POSTFIX d )

target_link_libraries(${AppName}  PRIVATE  ${NNG_LIBRARY}) 

target_include_directories(${AppName} PUBLIC 
	${CMAKE_CURRENT_LIST_DIR}/spdlog/include 
	../../thirdparty/Eigen
	../../thirdparty/crccpp/inc
	../../src)

