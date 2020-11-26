-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * --

group			"vendor"
project			"imgui"
targetname 		"%{prj.name}"
targetdir		"%{wks.location}/bin-lib/%{cfg.platform}/%{cfg.buildcfg}/"
objdir			"%{wks.location}/bin-obj/%{cfg.platform}/%{cfg.buildcfg}/"
location		"%{wks.location}/project/%{_ACTION}/vendor/%{prj.name}/"
debugdir 		"%{wks.location}/bin/%{cfg.platform}/%{cfg.buildcfg}/"
kind			"SharedLib"
language		"C++"
cppdialect 		"C++17"
staticruntime	"Off"
systemversion	"latest"

dependson{
	"imgui",
	"glfw",
}

defines{
	"_CRT_SECURE_NO_WARNINGS", "NOMINMAX",

	"IMGUI_API=__declspec(dllexport)",
	"IMGUI_IMPL_OPENGL_LOADER_GLEW",
}

undefines{
	"NDEBUG",
}

libdirs{
	"%{wks.location}/bin-lib/",
	"%{wks.location}/bin-lib/",
	"%{wks.location}/bin-lib/%{cfg.platform}/",
	"%{wks.location}/bin-lib/%{cfg.platform}/%{cfg.buildcfg}/",
	"%{wks.location}/vendor/bin-lib/",
	"%{wks.location}/vendor/bin-lib/",
	"%{wks.location}/vendor/bin-lib/%{cfg.platform}/",
	"%{wks.location}/vendor/bin-lib/%{cfg.platform}/%{cfg.buildcfg}/",
}

links{
	"opengl32",
	"glfw",
}

includedirs{
	"%{wks.location}/source",
	"%{wks.location}/vendor/source",
	"%{wks.location}/vendor/source/glfw/include",
	"%{wks.location}/vendor/source/imgui",
	"%{wks.location}/vendor/source/imgui-node-editor",
}

files{
	"%{wks.location}/vendor/build/%{prj.name}.**",
	
	"%{wks.location}/vendor/source/imgui/*.h",
	"%{wks.location}/vendor/source/imgui/*.cpp",
	"%{wks.location}/vendor/source/imgui/backends/imgui_impl_glfw.h",
	"%{wks.location}/vendor/source/imgui/backends/imgui_impl_glfw.cpp",
	"%{wks.location}/vendor/source/imgui/backends/imgui_impl_opengl3.h",
	"%{wks.location}/vendor/source/imgui/backends/imgui_impl_opengl3.cpp",
	
	"%{wks.location}/vendor/source/imgui_club/imgui_memory_editor/imgui_memory_editor.h",
	"%{wks.location}/vendor/source/ImGuizmo/ImGuizmo.h",
	"%{wks.location}/vendor/source/ImGuizmo/ImGuizmo.cpp",
	--"%{wks.location}/vendor/source/ImGuiColorTextEdit/*.h",
	--"%{wks.location}/vendor/source/ImGuiColorTextEdit/*.cpp",
	--"%{wks.location}/vendor/source/imgui-node-editor/*.h",
	--"%{wks.location}/vendor/source/imgui-node-editor/*.cpp",
	--"%{wks.location}/vendor/source/implot/**.h",
	--"%{wks.location}/vendor/source/implot/**.cpp",
}

postbuildcommands{
	"%{ml_copy} %{wks.location}\\bin-lib\\%{cfg.platform}\\%{cfg.buildcfg}\\%{prj.name}%{ml_dll} %{wks.location}\\bin\\%{cfg.platform}\\%{cfg.buildcfg}\\",
}
	
filter{ "configurations:Debug" }
	symbols "On" 
	prebuildcommands{
	}
	links{
		"glew32d",
	}

filter{ "configurations:Release" }
	optimize "Speed"
	prebuildcommands{
	}
	links{
		"glew32",
	}

-- WINDOWS

filter{ "system:Windows" }
	buildoptions{
		"/bigobj"
	}

filter{ "system:Windows", "configurations:Debug" }
	linkoptions{
		"/NODEFAULTLIB:MSVCRT.lib", "/NODEFAULTLIB:LIBCMT.lib", "/NODEFAULTLIB:LIBCMTD.lib"
	}

filter{ "system:Windows", "configurations:Release" }
	linkoptions{
		"/NODEFAULTLIB:LIBCMT.lib"
	}

-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * --