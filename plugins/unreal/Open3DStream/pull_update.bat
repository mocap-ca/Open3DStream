SET SRC_PROJECT=D:\P4_PD\o3ds

SET S=%SRC_PROJECT%\Plugins\Open3DStream\Source\Open3DStream

XCOPY /S /Y "%S%\Private\*.cpp" "%~DP0Source\Open3DStream\Private\"
XCOPY /S /Y "%S%\Public\*.h" "%~DP0Source\Open3DStream\Public\"
COPY /Y "%S%\Open3DStream.Build.cs" "%~DP0Source\Open3DStream\Open3DStream.Build.cs"
pause