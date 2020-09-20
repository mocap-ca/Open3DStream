@ECHO OFF
CALL ..\bin\build_env.bat

if NOT EXIST flatbuffers\CMakeLists.txt git submodule init
if NOT EXIST nng\CMakeLists.txt git submodule init

git submodule update

mkdir build
cd build

cmake -H.. -B. -G %CMAKE_VS_VERSION% -A x64
devenv O3DS3rdParty.sln /Build Debug /Project INSTALL 
devenv O3DS3rdParty.sln /Build Release /Project INSTALL 

cd %~DP0

