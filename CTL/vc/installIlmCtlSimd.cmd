@echo off
set src=..\..\..\..\IlmCtlSimd
cd %src%
set instpath=..\..\..\Deploy\include
mkdir %instpath%
copy *.h %instpath%

