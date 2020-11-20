@echo off
cd .\bin\x64\Debug\
start modus_launcher.exe %*
if %ERRORLEVEL% NEQ 0 ( pause )