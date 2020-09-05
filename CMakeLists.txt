cmake_minimum_required(VERSION  3.13)

project(Open3DStream)

set(CMAKE_DEBUG_POSTFIX d)

# Build the flatbuffer schema
set(schema_in ${CMAKE_CURRENT_LIST_DIR}/protocol/schema.fbs) 
set(schema_out ${CMAKE_CURRENT_LIST_DIR}/include/schema_generated.h)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include ${CMAKE_CURRENT_LIST_DIR}/src)
link_directories( ${CMAKE_CURRENT_LIST_DIR}/lib)		

# Linking nng static lib, so need to define this otherwise we will get linker errors.
# see - thirdparty\nng\include\nng\nng.h:32
add_definitions(-DNNG_STATIC_LIB)    

add_custom_command(
    OUTPUT ${schema_out}
    COMMAND ${CMAKE_CURRENT_LIST_DIR}/bin/flatc -o ${CMAKE_CURRENT_LIST_DIR}/include --cpp ${schema_in} 
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/protocol/schema.fbs
)

add_custom_target(protocol DEPENDS ${schema_out})
          
add_subdirectory(src)     
add_subdirectory(plugins/mobu)

# add_dependencies(open3dstream protocol)
									  								  
#add_executable(fbx_streamer {CMAKE_CURRENT_LIST_DIR}/apps/FbxStream/main.cpp
#	{CMAKE_CURRENT_LIST_DIR}/apps/FbxStream/fbxloader.cpp)


#target_link_libraries(fbx_streamer PRIVATE ${libs}
#	debug ${CMAKE_CURRENT_SOURCE_DIR}/../fbxsdk/lib/gcc4/x64/debug/fbxsdk.a
#	optimized ${CMAKE_CURRENT_SOURCE_DIR}/../fbxsdk/lib/gcc4/x64/release/fbxsdk.a)

#target_link_libraries(o3ds_pingServer PRIVATE ${libs})
										   
#target_link_libraries(o3ds_ping PRIVATE ${libs})
