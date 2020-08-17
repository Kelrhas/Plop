workspace "TD"
	startproject "TD"

include "../"

project "TD"
	kind "ConsoleApp" -- WindowedApp
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	
	location "./"
	debugdir "../"
	
	pchheader "TD_pch.h"
	pchsource "%{prj.location}/src/TD_pch.cpp"
	
	files
	{
		"%{prj.location}/src/**.h",
		"%{prj.location}/src/**.cpp"
	}
	
	includedirs
	{
		"src/",
		"../Plop/src/",
		"../Externals/ImGui/",
		"../Externals/glm/",
		"../Externals/optick/include/"
	}

	libdirs
	{
		"../Externals/optick/lib/x64/%{cfg.buildcfg}/"
	}
	
	links
	{
		"Plop",
		"OptickCore.lib"
	}
	