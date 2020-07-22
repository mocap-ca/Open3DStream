@ECHO OFF
cd %~dp0protocol
..\thirdparty\flatbuffers\build_release\flatc --cpp schema.fbs
IF %ERRORLEVEL% == 0 GOTO OKAY
pause
:OKAY