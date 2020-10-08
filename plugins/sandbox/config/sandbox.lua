-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * --

group			"plugins"
project			"sandbox"
targetname 		"%{prj.name}"
targetdir		"%{wks.location}/%{prj.name}/bin-lib/%{_ACTION}/%{cfg.platform}/%{cfg.buildcfg}plugins/"
objdir			"%{wks.location}/%{prj.name}/bin-obj/%{_ACTION}/%{cfg.platform}/%{cfg.buildcfg}plugins/"
location		"%{wks.location}/project/%{_ACTION}/plugins/"
debugdir 		"%{wks.location}/bin/%{_ACTION}/%{cfg.platform}/%{cfg.buildcfg}/"
kind			"SharedLib"
language		"C++"
cppdialect 		"C++17"
staticruntime	"Off"
rtti			"Off"
systemversion	"latest"

dependson{
	"launcher",
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
	"core",
}

includedirs{
	"%{wks.location}/source",
	"%{wks.location}/source/modus",
	"%{wks.location}/vendor/source",
	"%{wks.location}/vendor/source/json/include",
	"%{wks.location}/vendor/source/pybind11/include",
	"%{wks.location}/vendor/source/cpython/Include",
	"%{wks.location}/vendor/source/cpython/Include/internal",
	"%{wks.location}/vendor/source/entt/src",
	"%{wks.location}/vendor/source/imgui",
	"%{wks.location}/vendor/source/imgui-node-editor/NodeEditor/Include",
}

files{
	"%{wks.location}/plugins/%{prj.name}/config/**.**",
	"%{wks.location}/plugins/%{prj.name}/source/**.**",
}

postbuildcommands{
	"%{ml_copy} %{wks.location}\\bin-lib\\%{_ACTION}\\%{cfg.platform}\\%{cfg.buildcfg}\\plugins\\%{prj.name}%{ml_dll} %{wks.location}\\bin\\%{_ACTION}\\%{cfg.platform}\\%{cfg.buildcfg}\\plugins\\",
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

filter{ "system:Windows" }
	includedirs{
		"%{wks.location}/vendor/source/cpython/PC",
	}

filter{ "system:Windows", "configurations:Debug" }
	linkoptions{
		"/NODEFAULTLIB:MSVCRT.lib", "/NODEFAULTLIB:LIBCMT.lib", "/NODEFAULTLIB:LIBCMTD.lib"
	}

filter{ "system:Windows", "configurations:Release" }
	linkoptions{
		"/NODEFAULTLIB:LIBCMT.lib"
	}

-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * --