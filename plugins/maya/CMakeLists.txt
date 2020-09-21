cmake_minimum_required (VERSION 2.8.11)
project(MayaStreamDevice)

set(SRCS 
	meshNode.cpp
	pingCommand.cpp
	plugin.cpp
	threadedDevice.cpp
	pingCommand.h
	threadedDevice.h)

if(EXISTS  "c:/cpp/api/maya/2020")
	add_library(O3DSMaya2020Device  SHARED ${SRCS} )
	target_link_libraries(O3DSMaya2020Device Foundation.lib OpenMaya.lib OpenMayaAnim.lib OpenMayaUI.lib opengl32.lib glu32.lib Ws2_32.lib open3dstreamstatic ${libs})
	target_include_directories(O3DSMaya2020Device PUBLIC "c:/cpp/api/maya/2020/include")
	target_link_directories(O3DSMaya2020Device PUBLIC "c:/cpp/api/maya/2020/lib")
	target_compile_definitions(O3DSMaya2020Device PUBLIC _DEBUG NT_PLUGIN _CRT_SECURE_NO_DEPRECATE REQUIRE_IOSTREAM)
	set_target_properties(O3DSMaya2020Device PROPERTIES SUFFIX ".mll")
	target_link_options(O3DSMaya2020Device PUBLIC /export:initializePlugin /export:uninitializePlugin)

if(EXISTS "c:/Program Files/Autodesk/maya2020/plugins")
	install(TARGETS O3DSMaya2020Device DESTINATION "C:/Program Files/Autodesk/maya2020/plugins")
endif()


endif()



