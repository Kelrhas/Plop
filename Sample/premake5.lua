
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
		"../Externals/json/",
		"../Externals/ImGui/",
		"../Externals/glm/",
		"../Externals/optick/include/",
		"../Externals/fmt/include/",
		"../Externals/entt/",
		"../Externals/imgui_entt_entity_editor/",
	}
	
	links
	{
		"Plop"
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
	