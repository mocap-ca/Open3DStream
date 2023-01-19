@ECHO OFF

CALL build_env.bat
if %errorlevel% neq 0 exit /b %errorlevel%

IF XX%CMAKE_VS_VERSION% NEQ XX GOTO VSCHECKED
ECHO "ERROR: Visual studio version was not set."
EXIT /B 1

:VSCHECKED

IF EXIST %~DP0usr GOTO USROK

cd thirdparty
build.bat
if %errorlevel% neq 0 exit /b %errorlevel%
cd %~DP0

:USROK

ECHO ON

cd %~DP0
IF NOT EXIST vsbuild mkdir vsbuild
cd vsbuild

cmake -H.. -B. -G %CMAKE_VS_VERSION% -A x64 -DCMAKE_PREFIX_PATH=%~DP0usr -DCMAKE_INSTALL_PREFIX=%~DP0usr
if %errorlevel% neq 0 exit /b %errorlevel%

devenv Open3DStream.sln /Build Debug /Project INSTALL 
if %errorlevel% neq 0 exit /b %errorlevel% 

devenv Open3DStream.sln /Build RelWithDebInfo /Project INSTALL 
if %errorlevel% neq 0 exit /b %errorlevel%

cd %~DP0

python package.py

