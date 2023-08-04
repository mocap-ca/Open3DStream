cmake_minimum_required (VERSION 3.13)
project(MobuStreamDevice)

find_package(nng CONFIG REQUIRED)

set(SRCS plugin.cpp layout.cpp device.cpp device.h layout.h mobuModel.h mobuModel.cpp)

function(build_mobu version path)
  if(NOT EXISTS ${path})
    message(STATUS "Could not find $path")
    return()
  endif()

  set(mobu_target O3DSMobu${version}Device)
  add_library(${mobu_target} SHARED ${SRCS} )
  
  target_link_libraries(${mobu_target} fbsdk.lib opengl32.lib glu32.lib Ws2_32.lib open3dstreamstatic ${NNG_LIBRARY})
  target_include_directories(${mobu_target} PUBLIC "${path}/include" ${CMAKE_CURENT_LIST_DIR}../src)
  target_link_directories(${mobu_target} PUBLIC "${path}/lib/x64")
  target_compile_definitions(${mobu_target} PUBLIC NNG_STATIC_LIB)
  foreach(mbdir "C:/Program Files/Autodesk/MotionBuilder ${version}/bin/x64"
	            "D:/Programs/Autodesk/MotionBuilder ${version}/bin/x64")

    install(TARGETS ${mobu_target} RUNTIME DESTINATION "plugins/mobu")
    set_target_properties(${mobu_target} PROPERTIES VS_DEBUGGER_COMMAND "${mbdir}/motionbuilder.exe")
  endforeach()
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

if(EXISTS  "M:/code/api/windows/mobu/2023/OpenRealitySDK")
build_mobu("2023" "M:/code/api/windows/mobu/2023/OpenRealitySDK")
endif()

