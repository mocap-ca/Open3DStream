cmake_minimum_required (VERSION 3.13)
project(MobuStreamDevice)

find_package(nng CONFIG REQUIRED)

set(SRCS plugin.cpp layout.cpp device.cpp device.h layout.h mobuModel.h mobuModel.cpp audio.h audio.cpp)

function(build_mobu version path)
  if(NOT EXISTS ${path})
    message(STATUS "Could not find $path")
    return()
  endif()

  set(mobu_target O3DSMobu${version}Device)
  add_library(${mobu_target} SHARED ${SRCS} )

  if(${version} GREATER_EQUAL "2024")
     target_compile_definitions(${mobu_target} PUBLIC ORSDKPRODUCT_VERSION=ORSDK${version} )
  endif()

  set(audio_libraries
          mmdevapi.lib
          winmm.lib
  )
  target_link_libraries(${mobu_target} fbsdk.lib opengl32.lib glu32.lib Ws2_32.lib open3dstreamstatic ${NNG_LIBRARY} ${audio_libraries})
  target_include_directories(${mobu_target} PUBLIC "${path}/OpenRealitySDK/include" ${CMAKE_CURENT_LIST_DIR}../src)
  target_link_directories(${mobu_target} PUBLIC "${path}/OpenRealitySDK/lib/x64" "${path}/OpenRealitySDK/lib/x64/HIK2016")
  target_compile_definitions(${mobu_target} PUBLIC NNG_STATIC_LIB)
  install(TARGETS ${mobu_target} RUNTIME DESTINATION "plugins/mobu")
  set_target_properties(${mobu_target} PROPERTIES VS_DEBUGGER_COMMAND "${path}/bin/x64/motionbuilder.exe")
endfunction()

# Query the registry to find subkeys (versions)
set(MOTIONBUILDER_REG_PREFIX "HKEY_LOCAL_MACHINE\\SOFTWARE\\Autodesk\\MotionBuilder")
execute_process(
  COMMAND reg query ${MOTIONBUILDER_REG_PREFIX}
  OUTPUT_VARIABLE MOTIONBUILDER_REG_OUTPUT
  ERROR_QUIET
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(MOTIONBUILDER_REG_OUTPUT)
  # Split the registry output into lines to extract version information
  string(REGEX MATCHALL "HKEY_LOCAL_MACHINE\\\\[^\n\r]+" MOTIONBUILDER_VERSIONS ${MOTIONBUILDER_REG_OUTPUT})

  if (MOTIONBUILDER_VERSIONS)
    # Loop through all found versions
    foreach (VERSION_KEY ${MOTIONBUILDER_VERSIONS})

      string(REPLACE "${MOTIONBUILDER_REG_PREFIX}\\" "" MOBU_VERSION ${VERSION_KEY})
      message(STATUS "Found MotionBuilder version: ${MOBU_VERSION}")
      
      # Query the InstallPath for each version
      GET_FILENAME_COMPONENT(MOTIONBUILDER_INSTALL_PATH "[${VERSION_KEY};InstallPath]" ABSOLUTE CACHE)

      if (MOTIONBUILDER_INSTALL_PATH)
        message(STATUS "MotionBuilder ${MOBU_VERSION} is installed at: ${MOTIONBUILDER_INSTALL_PATH}")
        build_mobu(${MOBU_VERSION} ${MOTIONBUILDER_INSTALL_PATH})
      else()
        message(WARNING "Failed to get the installation path for MotionBuilder version: ${MOBU_VERSION}")
      endif()
    endforeach()
  else()
      message(WARNING "No Autodesk MotionBuilder versions found in the registry.")
  endif()
else()
  message(WARNING "Autodesk MotionBuilder is not installed or registry key is missing.")
endif()
