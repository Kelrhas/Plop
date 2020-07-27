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
	
	cppfiles =
	{
		"%{prj.location}/src/**.h",
		"%{prj.location}/src/**.cpp"
	}
	
	files
	{
		"Externals/json/nlohmann_json.natvis"
	}

	objdir "intermediate/%{prj.name}/"
	
	filter "configurations:Debug"
		defines { "_DEBUG" }
		symbols "On"
		targetsuffix "_d"

	filter "configurations:Release"
		defines { "NDEBUG", "_RELEASE" }
		symbols "On"
		optimize "Debug"
		targetsuffix "_r"

	filter "configurations:Master"
		defines { "_MASTER" }
		optimize "Speed"
		
	filter "system:Windows"
		systemversion "latest"
		

	includedirs {}

	libdirs {}

	characterset("MBCS") --Unicode

project "Plop"
	kind "StaticLib" --SharedLib
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	
	files (cppfiles)
	
	location "%{prj.name}"
	targetdir "bin/"
	
	pchheader "Plop_pch.h"
	pchsource "%{prj.location}/src/Plop_pch.cpp"
	
	includedirs
	{
		"%{prj.name}/src/",
		"Externals/json/",
		"Externals/glew/include"
	}
	
	libdirs
	{
		"Externals/glew/lib/"
	}

	links
	{
		"Externals/glew/lib/glew32s.lib",
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
	
	files (cppfiles)
	
	location "%{prj.name}"
	targetdir "bin/"
	debugdir "./"
	
	includedirs
	{
		"Plop/src/",
	}
	
	links { "Plop" }
	