!include "MUI2.nsh"
!include "WordFunc.nsh"
!verbose 3

Name "O3DS"

OutFile "${OUT_FILE}"
Unicode True

InstallDir "$LOCALAPPDATA\O3DS"
InstallDirRegKey HKCU "Software\O3DS" ""
RequestExecutionLevel admin

Var MobuInstall2020
Var MobuInstall2022
Var MobuInstall2023
Var MobuInstall2024

;Pages

  !insertmacro MUI_PAGE_LICENSE "LICENSE"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_INSTFILES  
  !insertmacro MUI_PAGE_FINISH
  
  !insertmacro MUI_LANGUAGE "English"
  
Section /o "Motion Builder 2020" SecMobu2020
  DetailPrint "Installing Motion Builder 2020 Plugin: $MobuInstall2020"
${If} $MobuInstall2020 == ""  
  SetOutPath $INSTDIR
 ${Else}
  SetOutPath "$MobuInstall2020"
${EndIf}
  File /oname=${FILE_VERSION}_2020.dll vsbuild\plugins\mobu\RelWithDebInfo\O3DSMobu2020Device.dll
SectionEnd  

Section /o "Motion Builder 2022" SecMobu2022
  DetailPrint "Installing Motion Builder 2022 Plugin: $MobuInstall2022"
${If} $MobuInstall2022 == ""  
  SetOutPath $INSTDIR
 ${Else}
  SetOutPath "$MobuInstall2022"
${EndIf}
  File /oname=${FILE_VERSION}_2022.dll vsbuild\plugins\mobu\RelWithDebInfo\O3DSMobu2022Device.dll
SectionEnd  

Section /o "Motion Builder 2023" SecMobu2023
  DetailPrint "Installing Motion Builder 2023 Plugin: $MobuInstall2023"
${If} $MobuInstall2023 == ""  
  SetOutPath $INSTDIR
 ${Else}
  SetOutPath "$MobuInstall2023"
${EndIf}
  File /oname=${FILE_VERSION}_2023.dll vsbuild\plugins\mobu\RelWithDebInfo\O3DSMobu2023Device.dll
SectionEnd  

Section /o "Motion Builder 2024" SecMobu2024
  DetailPrint "Installing Motion Builder 2024 Plugin: $MobuInstall2024"
${If} $MobuInstall2024 == ""  
  SetOutPath $INSTDIR
 ${Else}
  SetOutPath "$MobuInstall2024"
${EndIf}
  File /oname=${FILE_VERSION}_2024.dll vsbuild\plugins\mobu\RelWithDebInfo\O3DSMobu2024Device.dll
SectionEnd  




Function .onInit

  ReadRegStr $0 HKLM64 "Software\Autodesk\MotionBuilder\2020" InstallPath
  IfErrors M2022 0
  IfFileExists $0bin\x64\motionbuilder.exe 0 M2022
  SectionGetFlags ${SecMobu2020} $1
  IntOp $1 $1 | ${SF_SELECTED}
  SectionSetFlags ${SecMobu2020} $1
  StrCpy $MobuInstall2020 "$0bin\x64\plugins"

  
M2022:

  ReadRegStr $0 HKLM64 "Software\Autodesk\MotionBuilder\2022" InstallPath
  IfErrors M2023 0
  IfFileExists $0bin\x64\motionbuilder.exe 0 M2023
  SectionGetFlags ${SecMobu2022} $1
  IntOp $1 $1 | ${SF_SELECTED}
  SectionSetFlags ${SecMobu2022} $1
  StrCpy $MobuInstall2022 "$0bin\x64\plugins"

M2023:

  ReadRegStr $0 HKLM64 "Software\Autodesk\MotionBuilder\2023" InstallPath
  IfErrors M2024 0
  IfFileExists $0bin\x64\motionbuilder.exe 0 M2024
  SectionGetFlags ${SecMobu2023} $1
  IntOp $1 $1 | ${SF_SELECTED}
  SectionSetFlags ${SecMobu2023} $1
  StrCpy $MobuInstall2023 "$0bin\x64\plugins"	
  
M2024:

  ReadRegStr $0 HKLM64 "Software\Autodesk\MotionBuilder\2024" InstallPath
  IfErrors ENDER 0
  IfFileExists $0bin\x64\motionbuilder.exe 0 ENDER
  SectionGetFlags ${SecMobu2024} $1
  IntOp $1 $1 | ${SF_SELECTED}
  SectionSetFlags ${SecMobu2024} $1
  StrCpy $MobuInstall2024 "$0bin\x64\plugins"  
  
ENDER:

FunctionEnd  