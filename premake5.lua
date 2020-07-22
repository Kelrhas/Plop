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
	
	objdir "%{prj.location}/intermediate/"
	
	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"
		targetsuffix "_d"

	filter "configurations:Release"
		defines { "NDEBUG", "RELEASE" }
		symbols "On"
		optimize "Debug"
		targetsuffix "_r"

	filter "configurations:Master"
		defines { "MASTER" }
		optimize "Speed"
		
	filter "system:Windows"
		systemversion "latest"
		
	libdirs {}
	includedirs {}

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
		"%{prj.name}/src/"
	}
		
	defines { "PLOP_BUILD" }
		
project "Sample"
	kind "ConsoleApp" -- WindowedApp
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	
	files (cppfiles)
	
	location "%{prj.name}"
	targetdir "bin/"
	
	includedirs
	{
		"Plop/src/",
		"Externals/"
	}
	
	links { "Plop" }
	