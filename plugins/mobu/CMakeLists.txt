cmake_minimum_required (VERSION 3.13)
project(MobuStreamDevice)

set(SRCS plugin.cpp layout.cpp device.cpp device.h layout.h mobuModel.h mobuModel.cpp)

function(build_mobu version path)

	set(mobu_target O3DSMobu${version}Device)
	add_library(${mobu_target}  SHARED ${SRCS} )
	target_link_libraries(${mobu_target}  fbsdk.lib opengl32.lib glu32.lib Ws2_32.lib open3dstreamstatic ${libs})
	target_include_directories(${mobu_target} PUBLIC "${path}/include")
	target_link_directories(${mobu_target} PUBLIC "${path}/lib/x64")
	if(EXISTS "C:/Program Files/Autodesk/MotionBuilder ${version}/bin/x64/plugins")
		install(TARGETS ${mobu_target} DESTINATION "C:/Program Files/Autodesk/MotionBuilder ${version}/bin/x64/plugins")
	endif()
	if(EXISTS "D:/Programs/Autodesk/MotionBuilder ${version}/bin/x64/plugins")
		install(TARGETS ${mobu_target} DESTINATION "D:/Programs/Autodesk/MotionBuilder ${version}/bin/x64/plugins")
	endif()

endfunction()

if(EXISTS  "M:/code/api/windows/mobu/2018/OpenRealitySDK")
build_mobu("2018" "M:/code/api/windows/mobu/2018/OpenRealitySDK")
endif()

if(EXISTS  "M:/code/api/windows/mobu/2019/OpenRealitySDK")
build_mobu("2019" "M:/code/api/windows/mobu/2019/OpenRealitySDK")
endif()

if(EXISTS  "M:/code/api/windows/mobu/2020/OpenRealitySDK")
build_mobu("2020" "M:/code/api/windows/mobu/2020/OpenRealitySDK")
endif()

if(EXISTS  "M:/code/api/windows/mobu/2022/OpenRealitySDK")
build_mobu("2022" "M:/code/api/windows/mobu/2022/OpenRealitySDK")
endif()

