@ECHO OFF

REM Check for visual studio
CL.exe
IF %ERRORLEVEL% == 0 GOTO DOBUILD

SET VSPATH=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat

IF NOT EXIST "%VSPATH%" GOTO NOPE

CALL "%VSPATH%"
CL.exe
IF %ERRORLEVEL% == 0 GOTO DOBUILD

:NOPE
ECHO "Could not find visual studio install"
EXIT /B

:DOBUILD

IF EXIST "C:\Program Files\CMake\bin\cmake.exe" SET PATH=C:\Program Files\CMake\bin;%PATH%

if NOT EXIST thirdparty\flatbuffers\CMakeLists.txt git submodule init
if NOT EXIST thirdparty\nng\CMakeLists.txt git submodule init

git submodule update


:FLATBUFFERS
cd thirdparty\flatbuffers
if NOT EXIST bld MKDIR bld
cd bld
cmake -H.. -B. -G "Visual Studio 15 2017" -A x64 -DCMAKE_INSTALL_PREFIX=%~DP0 -DCMAKE_DEBUG_POSTFIX=d 
devenv FlatBuffers.sln /Build Debug /Project INSTALL 
devenv FlatBuffers.sln /Build Release /Project INSTALL 

cd %~DP0protocol
%~DP0bin\flatc --cpp schema.fbs
move schema_generated.h %~DP0include
cd %~DP0

:NNG
cd %~DP0thirdparty\nng
if NOT EXIST build MKDIR build
cd build 
REM cmake -H.. -B. -G "Visual Studio 15 2017" -A x64 -DCMAKE_INSTALL_PREFIX=E:\git\github\Open3DStream -DCMAKE_DEBUG_POSTFIX=d
cmake -H.. -B. -G "Visual Studio 15 2017" -A x64 -DCMAKE_INSTALL_PREFIX=%~DP0  -DCMAKE_DEBUG_POSTFIX=d
IF %ERRORLEVEL% NEQ  0 EXIT /B
devenv nng.sln /Build Debug /Project INSTALL 
devenv nng.sln /Build Release /Project INSTALL 

cd %~DP0

