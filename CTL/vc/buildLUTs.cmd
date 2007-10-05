@echo off

if exist ..\..\..\..\IlmCtlSimd\halfExpLogTable.h goto skip

set intdir=%1%
if "%intdir%"=="" set intdir=Release

cl /nologo /DOPENEXR_DLL /GR /GX /I..\..\..\..\..\..\Deploy\include ..\..\..\..\IlmCtlSimd\halfExpLogTable.cpp /MD /FehalfExpLogTable.exe /link "..\..\..\..\..\..\Deploy\lib\Release\Half.lib"
copy ..\..\..\..\..\..\Deploy\bin\%intdir%\Half.dll .
.\halfExpLogTable.exe ..\..\..\..\IlmCtlSimd\halfExpLogTable.h ..\..\..\..\IlmCtlSimd\halfExpLog.h
del Half.dll
del .\halfExpLogTable.obj .\halfExpLogTable.exe

:skip
