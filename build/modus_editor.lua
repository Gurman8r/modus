-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * --

group			""
project			"modus_editor"
targetname 		"%{prj.name}"
targetdir		"%{wks.location}/bin-lib/%{cfg.platform}/%{cfg.buildcfg}/"
objdir			"%{wks.location}/bin-obj/%{cfg.platform}/%{cfg.buildcfg}/"
location		"%{wks.location}/project/%{_ACTION}/modus/%{prj.name}/"
debugdir 		"%{wks.location}/bin/%{cfg.platform}/%{cfg.buildcfg}/"
kind			"SharedLib"
language		"C++"
cppdialect 		"C++17"
staticruntime	"Off"
rtti			"Off"
systemversion	"latest"

dependson{
	"modus_core",
}

defines{
	"_CRT_SECURE_NO_WARNINGS", "NOMINMAX",

	"ML_API=ML_API_EXPORT",
	"ML_IMPL_WINDOW_GLFW",
	"ML_IMPL_RENDERER_OPENGL",
	"ML_IMPL_OPENGL_LOADER_GLEW",
	--"ML_IMPL_OPENGL_LOADER_GLAD",
	"IMGUI_API=__declspec(dllimport)",
}

undefines{
	"NDEBUG",
}

debugenvs{
	"%{wks.location}/bin/%{cfg.platform}/%{cfg.buildcfg}/",
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
	"imgui",
	"modus_core",
}

includedirs{
	"%{wks.location}/source",
	"%{wks.location}/source/modus_core",
	"%{wks.location}/source/modus_editor",
	"%{wks.location}/vendor/source",
	"%{wks.location}/vendor/source/glfw/include",
	"%{wks.location}/vendor/source/assimp/include",
	"%{wks.location}/vendor/source/freetype2/include",
	"%{wks.location}/vendor/source/freetype2/include/freetype",
	"%{wks.location}/vendor/source/json/include",
	"%{wks.location}/vendor/source/pybind11/include",
	"%{wks.location}/vendor/source/cpython/Include",
	"%{wks.location}/vendor/source/cpython/Include/internal",
	"%{wks.location}/vendor/source/cpython/PC",
	"%{wks.location}/vendor/source/entt/src",
	"%{wks.location}/vendor/source/imgui",
	"%{wks.location}/vendor/source/imgui-node-editor/NodeEditor/Include",
}

files{
	"%{wks.location}/build/%{prj.name}.**",
	"%{wks.location}/resource/%{prj.name}.**",
	"%{wks.location}/resource/%{prj.name}/**.**",
	"%{wks.location}/source/%{prj.name}/**.**",
}

postbuildcommands{
	"%{ml_copy} %{wks.location}\\bin-lib\\%{cfg.platform}\\%{cfg.buildcfg}\\%{prj.name}%{ml_dll} %{wks.location}\\bin\\%{cfg.platform}\\%{cfg.buildcfg}\\",
}

filter{ "configurations:Debug" }
	symbols "On"
	links{
		"glew32d",
		"python39_d",
	}

filter{ "configurations:Release" }
	optimize "Speed"
	links{
		"glew32",
		"python39",
	}

-- WINDOWS

filter{ "system:Windows", "configurations:Debug" }
	linkoptions{
		"/NODEFAULTLIB:MSVCRT.lib", "/NODEFAULTLIB:LIBCMT.lib", "/NODEFAULTLIB:LIBCMTD.lib"
	}

filter{ "system:Windows", "configurations:Release" }
	linkoptions{
		"/NODEFAULTLIB:LIBCMT.lib"
	}

-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * --