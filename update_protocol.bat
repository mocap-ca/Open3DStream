@ECHO OFF
cd %~dp0
%~dp0\usr\bin\flatc --cpp src\o3ds.fbs
IF %ERRORLEVEL% NEQ 0 GOTO FAIL

move o3ds_generated.h ..\include
IF %ERRORLEVEL% NEQ 0 GOTO FAIL

ECHO Update successful.  Have a nice day!
pause
EXIT /B

:FAIL
ECHO Update failed - sorry!

pause




