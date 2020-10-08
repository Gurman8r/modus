@echo off
cd ..\
call .\vendor\bin\premake5\Windows\premake5.exe vs2017
if %ERRORLEVEL% NEQ 0 ( pause )