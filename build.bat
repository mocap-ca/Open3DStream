@ECHO OFF

CALL bin\build_env.bat
if %errorlevel% neq 0 exit /b %errorlevel%

cd %~DP0
if NOT EXIST build_release MKDIR build_release
cd build_release
cmake -H.. -B. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 exit /b %errorlevel%
nmake
if %errorlevel% neq 0 exit /b %errorlevel%

cd %~DP0
if NOT EXIST build_debug mkdir build_debug
cd build_debug
cmake -H.. -B. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Debug
if %errorlevel% neq 0 exit /b %errorlevel%
nmake
if %errorlevel% neq 0 exit /b %errorlevel%

cd %~DP0


