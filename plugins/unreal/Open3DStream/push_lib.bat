@ECHO OFF

SET SRC_PROJECT=M:\CLIENTS\Shocap\2022_10_28-TcpIssues\LiveSwitch-O3DS-TCPRelay-Test

REM SET DST=D:\Unreal Projects\O3DS_UE5\Plugins\Open3DStream
SET DST=%SRC_PROJECT%\Plugins\Open3DStream

SET BLD=%~DP0..\..\..\vsbuild

IF EXIST "%DST%\Open3DStream.uplugin" GOTO POK
ECHO Invalid destination dir: %DST%
PAUSE
EXIT /B

:POK

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

COPY "%BLD%\src\RelWithDebInfo\open3dstreamstatic.lib"  "%DST%"\lib
COPY "%BLD%\src\RelWithDebInfo\open3dstreamstatic.pdb"  "%DST%"\lib
REM COPY "%BLD%\src\RelWithDebInfo\open3dstreamstatic.bsr"  "%DST%"\lib

COPY "%~DP0..\..\..\thirdparty\build\nng\RelWithDebInfo\nng.lib" "%DST%"\lib
COPY "%~DP0..\..\..\thirdparty\build\flatbuffers\RelWithDebInfo\flatbuffers.lib" "%DST%"\lib

COPY "%~DP0..\..\..\src\o3ds\*.h" "%DST%\lib\include\o3ds\"
XCOPY /S /I /Y "%~DP0..\..\..\include\nng" "%DST%\lib\include\nng\"
XCOPY /S /I /Y "%~DP0..\..\..\include\flatbuffers" "%DST%\lib\include\flatbuffers\"
COPY "%~DP0..\..\..\include\*.h" "%DST%\lib\include\"

pause