@echo off
set deploypath=..\..\..\..\..\..\Deploy
set src=..\..\..\..\IlmImfCtl

if not exist %deploypath% mkdir %deploypath%

set intdir=%1%
if %intdir%=="" set intdir=Release
echo Installing into %intdir%
set instpath=%deploypath%\lib\%intdir%
if not exist %instpath% mkdir %instpath%

copy ..\%intdir%\IlmImfCtl.lib %instpath%
copy ..\%intdir%\IlmImfCtl.exp %instpath%

set instpath=%deploypath%\bin\%intdir%
if not exist %instpath% mkdir %instpath%
copy ..\%intdir%\IlmImfCtl.dll %instpath%

cd %src%
set instpath=..\..\..\Deploy\include
if not exist %instpath% mkdir %instpath%
copy *.h %instpath%

