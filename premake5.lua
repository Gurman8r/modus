-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * --
-- Workspace
-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * --

workspace "modus_%{_ACTION}"
	startproject "modus_launcher"
	configurations{ "Debug", "Release" }
	platforms{ "x86", "x64" }
	filter{ "platforms:*64" }
		architecture "x64"
	filter{ "platforms:*32" }
		architecture "x86"
	filter{ "platforms:*86" }
		architecture "x86"

-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * --
-- Commands
-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * --

filter{ "system:Unix", "system:Mac" }
	ml_copy	= "cp -rf"
	ml_move	= "mv -f"
	ml_exe	= ".o"
	ml_dll	= ".so"
	ml_lib	= ".a"
	
filter{ "system:Windows" }
	ml_copy	= "xcopy /y"
	ml_move	= "move /y"
	ml_exe	= ".exe"
	ml_dll	= ".dll"
	ml_lib	= ".lib"

-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * --
-- Projects
-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * --

dofile "./vendor/build/glfw.lua"
dofile "./vendor/build/imgui.lua"
dofile "./build/modus_core.lua"
dofile "./build/modus_launcher.lua"
--dofile "./plugins/sandbox/build/sandbox.lua"
		
-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * --