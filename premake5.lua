workspace "Plop"
	architecture "x86_64"
	startproject "Sample"
	
	configurations
	{
		"Debug",
		"Release",
		"Master"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

	objdir "intermediate/%{prj.name}/"
	targetdir "bin/"
	
	filter "configurations:Debug"
		defines { "_DEBUG" }
		runtime "Debug"
		symbols "On"
		targetsuffix "_d"

	filter "configurations:Release"
		defines { "NDEBUG", "_RELEASE" }
		runtime "Release"
		symbols "On"
		optimize "Debug"
		targetsuffix "_r"

	filter "configurations:Master"
		defines { "_MASTER" }
		runtime "Release"
		optimize "Speed"
		
	filter "system:Windows"
		systemversion "latest"

	characterset("MBCS") --Unicode

include "Externals/imgui"


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
		"Externals/json/nlohmann_json.natvis"
	}
	
	includedirs
	{
		"%{prj.name}/src/",
		"Externals/json/",
		"Externals/glew/include",
		"Externals/ImGui/",
		"Externals/glm/"
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
		"Externals/glm/"
	}
	
	links { "Plop" }
	