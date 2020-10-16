@echo off
cd .\bin\x86\Release\
start modus_launcher.exe %*
if %ERRORLEVEL% NEQ 0 ( pause )