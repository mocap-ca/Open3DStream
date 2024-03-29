@ECHO OFF
CALL %~DP0..\build_env.bat

if NOT EXIST flatbuffers\CMakeLists.txt git submodule init
if NOT EXIST nng\CMakeLists.txt git submodule init
if NOT EXIST websocketpp\CMakeLists.txt git submodule init

REM git submodule update --recursive

if NOT EXIST build mkdir build
cd build

cmake -H.. -B. -G %CMAKE_VS_VERSION% -A x64 -DCMAKE_INSTALL_PREFIX=%~DP0..\usr
devenv O3DS3rdParty.sln /Build Debug /Project INSTALL 
devenv O3DS3rdParty.sln /Build Release /Project INSTALL 

cd %~DP0

