SET S=C:\Users\Al\Documents\Unreal Projects\Open3dStream1\Plugins\Open3DStream\Source\Open3DStream

XCOPY /S /Y "%S%\Private\*.cpp" "%~DP0Source\Open3DStream\Private\"
XCOPY /S /Y "%S%\Public\*.h" "%~DP0Source\Open3DStream\Public\"

pause