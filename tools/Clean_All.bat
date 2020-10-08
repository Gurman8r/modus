@echo off

call .\Clean_Projects.bat
call .\Clean_Caches.bat
call .\Clean_Binaries.bat

if %ERRORLEVEL% NEQ 0 ( pause ) else ( echo all OK )