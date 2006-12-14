@echo off
set src=..\..\..\..\IlmCtlMath
cd %src%
set instpath=..\..\..\Deploy\include
mkdir %instpath%
copy *.h %instpath%

