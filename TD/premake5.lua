
project "TD"
	kind "ConsoleApp" -- WindowedApp
	language "C++"
	cppdialect "C++20"
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
		"../Externals/json/",
		"../Externals/ImGui/",
		"../Externals/glm/",
		"../Externals/optick/include/",
		"../Externals/fmt/include/",
		"../Externals/entt/",
		"../Externals/imgui_entt_entity_editor/",
		"../Externals/openal/include"
	}

	libdirs
	{
		"../Externals/optick/lib/x64/%{cfg.buildcfg}/",
		"../Externals/openal/libs/Win64/"
	}
	
	links
	{
		"Plop",
		"OpenAL32.lib"
	}

	filter "configurations:not Master"
		libdirs
		{
			"../Externals/optick/lib/x64/%{cfg.buildcfg}/"
		}
		links
		{
			"OptickCore.lib"
		}

	filter {}