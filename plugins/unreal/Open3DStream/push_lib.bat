SET DST=C:\Users\Al\Documents\Unreal Projects\Open3dStream1\Plugins\Open3DStream\lib

COPY "%~DP0..\..\..\lib\*.lib"  "%DST%"
COPY "%~DP0..\..\..\lib\*.sbr"  "%DST%"

COPY "%~DP0..\..\..\src\o3ds\*.h" "%DST%\include\o3ds\"

pause