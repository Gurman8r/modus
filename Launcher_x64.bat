@echo off
cd .\bin\x64\Release\
start modus_launcher.exe %*
if %ERRORLEVEL% NEQ 0 ( pause )