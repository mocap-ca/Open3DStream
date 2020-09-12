REM Set the build environment (Visual Studio 2017 and cmake)

REM Check for visual studio
CL.exe
IF %ERRORLEVEL% == 0 GOTO VSOK

SET CMAKE_VS_VERSION="Visual Studio 15 2107"
SET VSPATH=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat
IF EXIST "%VSPATH%" GOTO VSOK

SET CMAKE_VS_VERSION="Visual Studio 16 2019"
SET VSPATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat
IF EXIST "%VSPATH%" GOTO VSOK

ECHO "Could not find visual studio install"
EXIT /B

:VSOK
CALL "%VSPATH%"
CL.exe
IF %ERRORLEVEL% == 0 GOTO CLOK

ECHO "Could not find cl.exe"
EXIT /B

:CLOK

IF EXIST "C:\Program Files\CMake\bin\cmake.exe" SET PATH=C:\Program Files\CMake\bin;%PATH%

SET OPEN3DSBLD=1
