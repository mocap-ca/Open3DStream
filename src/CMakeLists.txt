cmake_minimum_required(VERSION  3.13)

project(Open3DStreamLib)

find_package(Flatbuffers CONFIG REQUIRED flatbuffers flatc HINTS ${CMAKE_CURRENT_LIST_DIR}/../usr)
find_package(nng CONFIG REQUIRED HINTS ${CMAKE_CURRENT_LIST_DIR}/../usr)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/_generated/o3ds_generated.h
    COMMAND flatbuffers::flatc -o ${CMAKE_CURRENT_BINARY_DIR}/_generated --cpp ${CMAKE_CURRENT_LIST_DIR}/o3ds.fbs
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/o3ds.fbs )

add_custom_target(protocol DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/_generated/o3ds_generated.h)

set(pub_headers			
	o3ds/o3ds.h
	o3ds/getTime.h
	o3ds/model.h
	o3ds/base_connector.h
	o3ds/nng_connector.h
	o3ds/subscriber.h
	o3ds/async_subscriber.h
	o3ds/async_publisher.h
	o3ds/pair.h
	o3ds/async_pair.h
	o3ds/publisher.h
	o3ds/async_request.h
	o3ds/transform_component.h
	o3ds/request.h
	o3ds/pipeline.h
	o3ds/async_pipeline.h
	o3ds/context.h
	o3ds/math.h
	o3ds/tcp.h
	o3ds/udp_fragment.h
	o3ds/socket_exception.h
	o3ds/socket_util.h
	o3ds/udp.h
	o3ds/socket_address.h
	o3ds/binary_stream.h
	o3ds/xsens_parser.h
)	

set(SOURCES 
	o3ds/o3ds.cpp
	o3ds/getTime.cpp
	o3ds/model.cpp	
	o3ds/base_connector.cpp
	o3ds/nng_connector.cpp
	o3ds/subscriber.cpp
	o3ds/async_subscriber.cpp
	o3ds/async_publisher.cpp
	o3ds/pair.cpp
	o3ds/async_pair.cpp
	o3ds/publisher.cpp
	o3ds/async_request.cpp
	o3ds/request.cpp
	o3ds/pipeline.cpp
	o3ds/async_pipeline.cpp
	o3ds/context.cpp
	o3ds/math.cpp
	o3ds/tcp.cpp
	o3ds/udp_fragment.cpp
	o3ds/socket_exception.cpp
	o3ds/socket_util.cpp
	o3ds/udp.cpp
	o3ds/socket_address.cpp
	o3ds/binary_stream.cpp
	o3ds/xsens_parser.cpp	
)
	
if(WIN32)
set(xtralibs wsock32 ws2_32)
endif()

# Shared library
# add_library(open3dstream SHARED ${SOURCES})
# target_link_libraries(open3dstream PRIVATE ${libs}  )
# add_dependencies(open3dstream protocol)

# Static linkable library
add_library(open3dstreamstatic STATIC ${SOURCES} ${pub_headers})

target_include_directories(open3dstreamstatic PUBLIC 
	${CMAKE_CURRENT_BINARY_DIR}/_generated ${CMAKE_CURRENT_LIST_DIR}
	${FLATBUFFERS_INCLUDE_DIRS} 
	${NNG_INCLUDE_DIRS}/..)

# target_link_libraries(open3dstreamstatic PRIVATE ${NNG_LIBRARIES} ${FLATBUFFERS_LIBRARIES} ${xtralibs})        
add_dependencies(open3dstreamstatic protocol)

list(JOIN pub_headers ";" pub_headers_str)
set_target_properties(open3dstreamstatic PROPERTIES PUBLIC_HEADER "${pub_headers_str}")

if(NOT DEFINED $VERSION_TAG)
	set(VERSION_TAG dev)
endif()
target_compile_definitions(open3dstreamstatic PUBLIC NNG_STATIC_LIB CRCPP_USE_NAMESPACE VERSION_TAG=${VERSION_TAG})

install(TARGETS open3dstreamstatic 
	LIBRARY DESTINATION lib
	PUBLIC_HEADER DESTINATION include/o3ds
	CONFIGURATIONS RelWithDebInfo)

install(FILES 	${CMAKE_CURRENT_BINARY_DIR}/_generated/o3ds_generated.h
	DESTINATION include)
