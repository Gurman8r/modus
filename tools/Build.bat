@echo off
cls
set CWD=%cd%\
pushd %CWD%

set solution=%1

rem 2015 / 2017 / 2019
set vs_version=%2

rem Enterprise / Community
set vs_edition=%3

rem Debug / Release
set config=%4

rem x86 / x64
set platform=%5

rem v140 / v141 / v142
set toolset=%6

rem Console / Windows
set subsystem=%7

echo Version 	Visual Studio %vs_version% %vs_edition%
echo Configure 	%config%
echo Platform 	%platform%
echo Toolset 	%toolset%
echo Subsystem 	%subsystem%

set msbuild_path=%ProgramFiles(x86)%\Microsoft Visual Studio\%vs_version%\%vs_edition%\

cd %msbuild_path%VC\Auxiliary\Build\
if "%platform%" == "x86" 	( call "vcvarsall.bat" x86 )
if "%platform%" == "x64" 	( call "vcvarsall.bat" x64 )
if "%platform%" == "Win32" 	( call "vcvarsall.bat" x86 )
if "%platform%" == "Win64" 	( call "vcvarsall.bat" x64 )

cd %msbuild_path%
call msbuild.exe ^
 "%CWD%..\%solution%" ^
 "/p:Configuration=%config%" ^
 "/p:PlatformTarget=%platform%" ^
 "/p:PlatformToolset=%toolset%" ^
 "/p:Subsystem=%subsystem%"

popd
if %ERRORLEVEL% NEQ 0 ( pause )