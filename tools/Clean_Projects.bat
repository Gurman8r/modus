@echo off

if exist ..\.vs rd /s /q ..\.vs
if exist ..\project rd /s /q ..\project

if exist ..\modus_vs2017.sln del /q ..\modus_vs2017.sln
if exist ..\modus_vs2019.sln del /q ..\modus_vs2019.sln

if %ERRORLEVEL% NEQ 0 ( pause ) else ( echo projects OK )