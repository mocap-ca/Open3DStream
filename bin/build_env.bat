REM Set the build environment (Visual Studio 2017 and cmake)

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

SET OPEN3DSBLD=1
