-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * --

group			"modus"
project			"launcher"
targetname 		"%{prj.name}"
targetdir		"%{wks.location}/bin-lib/%{_ACTION}/%{cfg.platform}/%{cfg.buildcfg}/"
objdir			"%{wks.location}/bin-obj/%{_ACTION}/%{cfg.platform}/%{cfg.buildcfg}/"
location		"%{wks.location}/project/%{_ACTION}/%{prj.name}/"
debugdir 		"%{wks.location}/bin/%{_ACTION}/%{cfg.platform}/%{cfg.buildcfg}/"
kind			"ConsoleApp"
language		"C++"
cppdialect 		"C++17"
staticruntime	"Off"
rtti			"Off"
systemversion	"latest"

dependson{
	"runtime",
}

defines{
	"_CRT_SECURE_NO_WARNINGS", "NOMINMAX",
}

undefines{
	"NDEBUG",
}

libdirs{
	"%{wks.location}/bin-lib/",
	"%{wks.location}/bin-lib/%{_ACTION}/",
	"%{wks.location}/bin-lib/%{_ACTION}/%{cfg.platform}/",
	"%{wks.location}/bin-lib/%{_ACTION}/%{cfg.platform}/%{cfg.buildcfg}/",
	"%{wks.location}/vendor/bin-lib/",
	"%{wks.location}/vendor/bin-lib/%{_ACTION}/",
	"%{wks.location}/vendor/bin-lib/%{_ACTION}/%{cfg.platform}/",
	"%{wks.location}/vendor/bin-lib/%{_ACTION}/%{cfg.platform}/%{cfg.buildcfg}/",
}

links{
	"glfw",
	"core",
}

includedirs{
	"%{wks.location}/source",
	"%{wks.location}/source/modus",
	"%{wks.location}/vendor/source",
	"%{wks.location}/vendor/source/glfw/include",
	"%{wks.location}/vendor/source/assimp/include",
	"%{wks.location}/vendor/source/freetype2/include",
	"%{wks.location}/vendor/source/freetype2/include/freetype",
	"%{wks.location}/vendor/source/json/include",
	"%{wks.location}/vendor/source/pybind11/include",
	"%{wks.location}/vendor/source/cpython/Include",
	"%{wks.location}/vendor/source/cpython/Include/internal",
	"%{wks.location}/vendor/source/entt/src",
	"%{wks.location}/vendor/source/imgui",
	"%{wks.location}/vendor/source/imgui-node-editor/NodeEditor/Include",
}

files{
	"%{wks.location}/config/%{prj.name}.**",
	"%{wks.location}/source/modus/%{prj.name}/**.**",
}

postbuildcommands{
	"%{ml_copy} %{wks.location}\\bin-lib\\%{_ACTION}\\%{cfg.platform}\\%{cfg.buildcfg}\\%{prj.name}%{ml_exe} %{wks.location}\\bin\\%{_ACTION}\\%{cfg.platform}\\%{cfg.buildcfg}\\",
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

-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * --

-- WINDOWS

filter{ "system:Windows" }
	includedirs{
		"%{wks.location}/vendor/source/cpython/PC",
	}

-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * --