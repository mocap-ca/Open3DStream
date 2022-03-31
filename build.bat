@ECHO On

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
IF NOT EXIST vsbuild mkdir vsbuild
cd vsbuild

cmake -H.. -B. -G %CMAKE_VS_VERSION% -A x64
if %errorlevel% neq 0 exit /b %errorlevel%

devenv Open3DStream.sln /Build Debug
if %errorlevel% neq 0 exit /b %errorlevel%

devenv Open3DStream.sln /Build Release 
if %errorlevel% neq 0 exit /b %errorlevel%

cd %~DP0


