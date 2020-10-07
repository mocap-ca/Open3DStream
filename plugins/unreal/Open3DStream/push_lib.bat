SET DST=%USERPROFILE%\Documents\Unreal Projects\Open3dStream1\Plugins\Open3DStream

IF EXIST "%DST%" GOTO DSTOK

ECHO could not find unreal project
PAUSE
EXIT /B

:DSTOK

IF NOT EXIST "%DST%"\lib MKDIR "%DST%"\lib
IF NOT EXIST "%DST%"\lib\include MKDIR "%DST%"\lib\include
IF NOT EXIST "%DST%"\lib\include\o3ds MKDIR "%DST%"\lib\include\o3ds
IF NOT EXIST "%DST%"\lib\include\nng MKDIR "%DST%"\lib\include\nng
IF NOT EXIST "%DST%"\lib\include\nng MKDIR "%DST%"\lib\include\flatbuffers

COPY "%~DP0..\..\..\build\src\RelWithDebInfo\*.lib"  "%DST%"\lib
COPY "%~DP0..\..\..\build\src\RelWithDebInfo\*.bsc"  "%DST%"\lib

COPY "%~DP0..\..\..\src\o3ds\*.h" "%DST%\lib\include\o3ds\"
XCOPY /S /I /Y "%~DP0..\..\..\include\nng" "%DST%\lib\include\nng\"
XCOPY /S /I /Y "%~DP0..\..\..\include\flatbuffers" "%DST%\lib\include\flatbuffers\"
COPY "%~DP0..\..\..\include\*.h" "%DST%\lib\include\"

pause