SET(CMAKE_MIN_REQ_VERSION 3.13)
cmake_minimum_required(VERSION ${CMAKE_MIN_REQ_VERSION})

project(FbxStream)

if (UNIX)
	set(fbxroot /home/ec2-user/fbxsdk)
	list(APPEND libs xml2 fbxsdk)
	if(CMAKE_BUILD_TYPE MATCHES Release OR CMAKE_BUILD_TYPE STREQUAL "")
	link_directories(${fbxroot}/lib/gcc/x64/release/)
	#install(FILES
	#		${fbxroot}/lib/gcc4/x64/release/libfbxsdk.a
	#		${fbxroot}/lib/gcc4/x64/release/libfbxsdk.so
	#		DESTINATION /usr/local/lib64/)
	endif()
	if(CMAKE_BUILD_TYPE MATCHES Debug)
	link_directories(${fbxroot}/lib/gcc/x64/debug/)
	endif()
endif()

if(WIN32)
	set(fbxroot "C:/Program Files/Autodesk/FBX/FBX SDK/2020.0.1") 

	# As we are adding full paths to the debug/release fbx libs, we need to set the CMP0003 policy
	list(APPEND libs 
		debug ${fbxroot}/lib/vs2017/x64/debug/libfbxsdk.lib  
		optimized ${fbxroot}/lib/vs2017/x64/release/libfbxsdk.lib 
	)
endif()

include_directories(${fbxroot}/include)


add_executable(FbxStream main.cpp fbxloader.cpp fbxloader.h)

message("LIBS ${libs}")
target_link_libraries(FbxStream PRIVATE ${libs} open3dstreamstatic)

if(WIN32)

add_custom_command(TARGET FbxStream POST_BUILD       
    COMMAND ${CMAKE_COMMAND} -E copy_if_different   
        ${fbxroot}/lib/vs2017/x64/$<$<CONFIG:RelWithDebInfo>:Release>$<$<NOT:$<CONFIG:RelWithDebInfo>>:$<CONFIG>>/libfbxsdk.dll     
        $<TARGET_FILE_DIR:FbxStream>)   

# https://cmake.org/cmake/help/git-stage/prop_tgt/VS_DEBUGGER_COMMAND_ARGUMENTS.html
# https://stackoverflow.com/questions/52649753/how-to-send-visual-studio-configuration-options-from-cmake
set_target_properties(FbxStream PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "${CMAKE_CURRENT_LIST_DIR}/../../test_data/beta_fight.fbx server ws://127.0.0.1:6001")

endif()

# install(TARGETS FbxStream DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin)

