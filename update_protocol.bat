@ECHO OFF
cd %~dp0protocol
%~dp0\bin\flatc --cpp schema.fbs
IF %ERRORLEVEL% NEQ 0 GOTO FAIL

move schema_generated.h ..\include
IF %ERRORLEVEL% NEQ 0 GOTO FAIL

ECHO Update successful.  Have a nice day!
cd %~dp0
EXIT /B

:FAIL
pause




