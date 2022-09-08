cmake_minimum_required (VERSION 3.13)
project(MayaStreamDevice)

set(SRCS 
	pingCommand.cpp
	plugin.cpp
	threadedDevice.cpp
	pingCommand.h
	threadedDevice.h)

set(MAYALIBS Foundation.lib OpenMaya.lib OpenMayaAnim.lib OpenMayaUI.lib opengl32.lib glu32.lib Ws2_32.lib open3dstreamstatic )

function(build_maya version)

	set(mayatarget O3DSMaya${version}Device)

	add_library(${mayatarget}  SHARED ${SRCS} )
	target_link_libraries(${mayatarget}  ${MAYALIBS} ${libs})
	target_include_directories(${mayatarget} PUBLIC "M:/code/api/windows/maya/${version}/include")
	target_link_directories(${mayatarget} PUBLIC "M:/code/api/windows/maya/${version}/lib")
	target_compile_definitions(${mayatarget} PUBLIC NT_PLUGIN _CRT_SECURE_NO_DEPRECATE REQUIRE_IOSTREAM)
	if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
		target_compile_definitions(${mayatarget} _DEBUG)
	endif()
	set_target_properties(${mayatarget} PROPERTIES SUFFIX ".mll")
	target_link_options(${mayatarget} PUBLIC /export:initializePlugin /export:uninitializePlugin)
	# set_property(TARGET O3DSMaya2020Device PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

	if(EXISTS "c:/Program Files/Autodesk/maya${version}/plugins")
	install(TARGETS ${mayatarget} DESTINATION "$ENV{USERPROFILE}/Documents/maya/2020/plug-ins")
	endif()
	
	install(TARGETS ${mayatarget} DESTINATION plugins/maya)
	install(FILES $<TARGET_PDB_FILE:${mayatarget}> DESTINATION plugins/maya OPTIONAL)


endfunction()

if(EXISTS "M:/code/api/windows/maya/2022")
build_maya("2022")
endif()

if(EXISTS "M:/code/api/windows/maya/2020_4")
build_maya("2020_4")
endif()

if(EXISTS "M:/code/api/windows/maya/2020")
build_maya("2020")
endif()

if(EXISTS "M:/code/api/windows/maya/2019_3")
build_maya("2019_3")
endif()

if(EXISTS "M:/code/api/windows/maya/2019")
build_maya("2019")
endif()

if(EXISTS "M:/code/api/windows/maya/2018_6")
build_maya("2018_6")
endif()

if(EXISTS "M:/code/api/windows/maya/2018")
build_maya("2018")
endif()








