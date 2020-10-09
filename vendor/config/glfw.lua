-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * --

group			"vendor"
project			"glfw"
targetname 		"%{prj.name}"
targetdir		"%{wks.location}/bin-lib/%{cfg.platform}/%{cfg.buildcfg}/"
objdir			"%{wks.location}/bin-obj/%{cfg.platform}/%{cfg.buildcfg}/"
location		"%{wks.location}/project/%{_ACTION}/%{prj.name}/vendor/%{prj.name}/"
debugdir 		"%{wks.location}/bin/%{cfg.platform}/%{cfg.buildcfg}/"
kind			"StaticLib"
language		"C++"
cppdialect 		"C++17"
staticruntime	"Off"
systemversion	"latest"

defines{
	"_CRT_SECURE_NO_WARNINGS", "NOMINMAX",
}

undefines{
	"NDEBUG",
	"__NULL_IMPORT_DESCRIPTOR",
}

includedirs{
	"%{wks.location}/vendor/source/glfw/include",
	"%{wks.location}/vendor/source/glfw/include",
	"%{wks.location}/vendor/source/glfw/src",
}

files{
	"%{wks.location}/vendor/config/%{prj.name}.**",
	"%{wks.location}/vendor/source/glfw/include/glfw/**.h",
	"%{wks.location}/vendor/source/glfw/src/context.c", 
	"%{wks.location}/vendor/source/glfw/src/egl_context.c",
	"%{wks.location}/vendor/source/glfw/src/egl_context.h",
	"%{wks.location}/vendor/source/glfw/src/init.c", 
	"%{wks.location}/vendor/source/glfw/src/input.c", 
	"%{wks.location}/vendor/source/glfw/src/internal.h", 
	"%{wks.location}/vendor/source/glfw/src/mappings.h", 
	"%{wks.location}/vendor/source/glfw/src/monitor.c", 
	"%{wks.location}/vendor/source/glfw/src/osmesa_context.c",
	"%{wks.location}/vendor/source/glfw/src/osmesa_context.h",
	"%{wks.location}/vendor/source/glfw/src/vulkan.c",
	"%{wks.location}/vendor/source/glfw/src/wgl_context.c",
	"%{wks.location}/vendor/source/glfw/src/wgl_context.h",
	"%{wks.location}/vendor/source/glfw/src/window.c",
}

libdirs{
	"%{wks.location}/vendor/bin-lib/",
	"%{wks.location}/vendor/bin-lib/",
	"%{wks.location}/vendor/bin-lib/%{cfg.platform}/",
	"%{wks.location}/vendor/bin-lib/%{cfg.platform}/%{cfg.buildcfg}/",
}

links{
	"opengl32",
}
	
filter{ "configurations:Debug" }
	symbols "On" 
	prebuildcommands{
		"%{ml_copy} %{wks.location}\\vendor\\bin\\%{cfg.platform}\\%{cfg.buildcfg}\\glew32d%{ml_dll} %{wks.location}\\bin\\%{cfg.platform}\\%{cfg.buildcfg}\\",
		"%{ml_copy} %{wks.location}\\vendor\\bin\\%{cfg.platform}\\%{cfg.buildcfg}\\python39_d%{ml_dll} %{wks.location}\\bin\\%{cfg.platform}\\%{cfg.buildcfg}\\",
	}
	links{
		"glew32d",
		"python39_d",
	}

filter{ "configurations:Release" }
	optimize "Speed"
	prebuildcommands{
		"%{ml_copy} %{wks.location}\\vendor\\bin\\%{cfg.platform}\\%{cfg.buildcfg}\\glew32%{ml_dll} %{wks.location}\\bin\\%{cfg.platform}\\%{cfg.buildcfg}\\",
		"%{ml_copy} %{wks.location}\\vendor\\bin\\%{cfg.platform}\\%{cfg.buildcfg}\\python39%{ml_dll} %{wks.location}\\bin\\%{cfg.platform}\\%{cfg.buildcfg}\\",
	}
	links{
		"glew32",
		"python39",
	}

-- WINDOWS

filter{ "system:Windows" }
	defines{
		"_GLFW_WIN32=1",
	}
	files{
		"%{wks.location}/vendor/source/glfw/src/win32_init.c",
		"%{wks.location}/vendor/source/glfw/src/win32_joystick.c",
		"%{wks.location}/vendor/source/glfw/src/win32_joystick.h",
		"%{wks.location}/vendor/source/glfw/src/win32_monitor.c",
		"%{wks.location}/vendor/source/glfw/src/win32_platform.h",
		"%{wks.location}/vendor/source/glfw/src/win32_thread.c",
		"%{wks.location}/vendor/source/glfw/src/win32_time.c",
		"%{wks.location}/vendor/source/glfw/src/win32_window.c",
	}
	
filter{ "system:Windows", "configurations:Debug" }
	linkoptions{
		"/NODEFAULTLIB:MSVCRT.lib",
	}

-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * --