@ECHO OFF

CALL bin\build_env.bat

IF NOT EXIST build MKDIR build
cd build

cmake -H.. -B. -G %CMAKE_VS_VERSION% -A x64
devenv Open3DStream.sln /Build Debug
devenv Open3DStream.sln /Build Release 