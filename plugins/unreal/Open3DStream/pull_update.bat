REM #SET S=C:\Users\Al\Documents\Unreal Projects\Open3dStream1\Plugins\Open3DStream\Source\Open3DStream
REM #SET S=D:\Unreal Projects\Open3dStream1\Plugins\Open3DStream\Source\Open3DStream

SET SRC_PROJECT=M:\CLIENTS\Shocap\2022_10_28-TcpIssues\LiveSwitch-O3DS-TCPRelay-Test

SET S=%SRC_PROJECT%\Plugins\Open3DStream\Source\Open3DStream

XCOPY /S /Y "%S%\Private\*.cpp" "%~DP0Source\Open3DStream\Private\"
XCOPY /S /Y "%S%\Public\*.h" "%~DP0Source\Open3DStream\Public\"
COPY /Y "%S%\Open3DStream.Build.cs" "%~DP0Source\Open3DStream\Open3DStream.Build.cs"
pause