workspace "Plop"
	architecture "x86_64"
	startproject "Sample"

	linkoptions
	{
		"/IGNORE:4099" -- for glew pdb
	}
	
	configurations
	{
		"Debug",
		"Release",
		"Master"
	}
	
	flags
	{
		"MultiProcessorCompile",
		"FatalWarnings",
		"ShadowedVariables"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	objdir "intermediate/%{prj.name}/"
	targetdir "bin/"
	
	filter "configurations:Debug"
		defines { "_DEBUG" }
		runtime "Debug"
		symbols "on"
		targetsuffix "_d"
		linkoptions
		{
			"/NODEFAULTLIB:libcmt.lib",
			"/NODEFAULTLIB:msvcrt.lib",
			"/NODEFAULTLIB:msvcrtd.lib"
		}

	filter "configurations:Release"
		defines { "NDEBUG", "_RELEASE" }
		runtime "Release"
		symbols "on"
		optimize "Debug"
		targetsuffix "_r"

	filter "configurations:Master"
		defines { "_MASTER" }
		runtime "Release"
		optimize "Speed"
		
	filter "system:Windows"
		systemversion "latest"
		defines { "PLATFORM_WINDOWS" }

	characterset("MBCS") --Unicode

include "Externals/imgui-premake"


project "Plop"
	kind "StaticLib" --SharedLib
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	
	location "%{prj.name}"
	
	pchheader "Plop_pch.h"
	pchsource "%{prj.location}/src/Plop_pch.cpp"
		
	files
	{
		"%{prj.location}/src/**.h",
		"%{prj.location}/src/**.cpp",
		"Externals/json/nlohmann_json.natvis",
		"Externals/glm/util/*",
		"Externals/stb/stb.cpp"
	}
	
	includedirs
	{
		"%{prj.name}/src/",
		"Externals/json/",
		"Externals/glew/include",
		"Externals/ImGui/",
		"Externals/glm/",
		"Externals/stb/"
	}
	
	libdirs
	{
		"Externals/glew/lib/"
	}

	links
	{
		"Externals/glew/lib/glew32s.lib",
		"ImGui",
		"opengl32.lib"
	}
		
	defines
	{
		"PLOP_BUILD",
		"GLEW_STATIC"
	}
		
project "Sample"
	kind "ConsoleApp" -- WindowedApp
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	
	location "%{prj.name}"
	debugdir "./"
	
	files
	{
		"%{prj.location}/src/**.h",
		"%{prj.location}/src/**.cpp"
	}
	
	includedirs
	{
		"Plop/src/",
		"%{prj.name}/src/",
		"Externals/ImGui/",
		"Externals/glm/"
	}
	
	links { "Plop" }
	