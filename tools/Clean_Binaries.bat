@echo off

if exist ..\bin rd /s /q ..\bin
if exist ..\bin-lib rd /s /q ..\bin-lib
if exist ..\bin-obj rd /s /q ..\bin-obj

if %ERRORLEVEL% NEQ 0 ( pause ) else ( echo binaries OK )