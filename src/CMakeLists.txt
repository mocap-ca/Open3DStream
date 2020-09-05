cmake_minimum_required(VERSION  3.13)

project(Open3DStreamLib)

#include_directories( ${CMAKE_CURRENT_LIST_DIR}/../include)
#link_directories( ${CMAKE_CURRENT_LIST_DIR}/../lib)		    
            
set(SOURCES 
	o3ds/getTime.cpp
	o3ds/subscriber.cpp
	o3ds/broadcaster.cpp
	o3ds/model.cpp
	)

set(libs wsock32 ws2_32 debug flatbuffersd optimized flatbuffers debug nngd optimized nng)

# Shared library
# add_library(open3dstream SHARED ${SOURCES})
#target_link_libraries(open3dstream PRIVATE ${libs}  )
# add_dependencies(open3dstream protocol)


# Static linkable library
add_library(open3dstreamstatic STATIC ${SOURCES})
target_link_libraries(open3dstreamstatic PRIVATE ${libs})        
add_dependencies(open3dstreamstatic protocol)
