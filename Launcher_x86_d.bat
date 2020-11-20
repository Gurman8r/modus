@echo off
cd .\bin\x86\Debug\
start modus_launcher.exe %*
if %ERRORLEVEL% NEQ 0 ( pause )