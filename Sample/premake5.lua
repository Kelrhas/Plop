workspace "Sample"
	startproject "Sample"

include "../"
		
project "Sample"
	kind "ConsoleApp" -- WindowedApp
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	
	location "./"
	debugdir "../"
		
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
	