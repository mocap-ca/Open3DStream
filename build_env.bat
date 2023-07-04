REM Set the build environment (Visual Studio 2019/2017 and cmake)

IF "%OPEN3DSBLD%" == "" GOTO DOBLD
ECHO Environment already set
EXIT /B 0

:DOBLD


REM Check for visual studio
CL.exe >NUL 2>NUL
IF %ERRORLEVEL% == 0 GOTO CLOK

SET CMAKE_VS_VERSION="Visual Studio 17 2022"
SET VSPATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat
IF EXIST "%VSPATH%" GOTO VSOK

SET CMAKE_VS_VERSION="Visual Studio 16 2019"
SET VSPATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat
IF EXIST "%VSPATH%" GOTO VSOK

SET CMAKE_VS_VERSION="Visual Studio 15 2017"
SET VSPATH=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat
IF EXIST "%VSPATH%" GOTO VSOK


ECHO "Could not find visual studio install"
EXIT /B 1

:VSOK
CALL "%VSPATH%"
CL.exe >NUL 2>NUL
IF %ERRORLEVEL% == 0 GOTO CLOK

ECHO "Could not find cl.exe"
EXIT /B 1

:CLOK

IF EXIST "C:\Program Files\CMake\bin\cmake.exe" SET PATH=C:\Program Files\CMake\bin;%PATH%

SET OPEN3DSBLD=1
