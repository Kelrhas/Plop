
project "Plop"
	kind "StaticLib" --SharedLib
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	
	location "./"
	
	pchheader "Plop_pch.h"
	pchsource "%{prj.location}/src/Plop_pch.cpp"
		
	files {
		"%{prj.location}/src/**.h",
		"%{prj.location}/src/**.cpp",
		"%{prj.location}/src/**.hpp",
		"%{prj.location}/src/**.inl",
		"../Externals/json/nlohmann_json.natvis",
		"../Externals/entt/entt/natvis/*.natvis",
		"../Externals/glm/util/*",
		"../Externals/stb/stb.cpp"
	}
	
	includedirs {
		"src/",
		"../Externals/json/",
		"../Externals/glew/include",
		"../Externals/ImGui/",
		"../Externals/ImGuizmo/",
		"../Externals/glm/",
		"../Externals/stb/",
		"../Externals/optick/include/",
		"../Externals/fmt/include/",
		"../Externals/entt/",
		"../Externals/imgui_entt_entity_editor/",
		"../Externals/openal/include"
	}
	
	libdirs {
		"../Externals/glew/lib/",
	}

	links {
		"../Externals/glew/lib/glew32s.lib",
		"ImGui",
		"opengl32.lib",
	}
		
	defines {
		"PLOP_BUILD",
		"GLEW_STATIC"
	}

	filter {}
