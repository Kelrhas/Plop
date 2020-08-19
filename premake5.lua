--workspace
	architecture "x86_64"

	linkoptions {
		"/IGNORE:4099" -- for glew pdb
	}
	
	configurations {
		"Debug",
		"Release",
		"Master"
	}
	
	flags {
		"MultiProcessorCompile",
		"FatalWarnings",
		"ShadowedVariables"
	}

	defines { "_CRT_SECURE_NO_WARNINGS"	}

	objdir "intermediate/%{prj.name}/"
	targetdir "bin/"
	
	filter "configurations:Debug"
		defines { "_DEBUG" }
		runtime "Debug"
		symbols "on"
		targetsuffix "_d"
		linkoptions {
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
		defines {
			"PLATFORM_WINDOWS",
			"NOMINMAX" -- removes the min and max macro to avoid conflict with std
		}

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
		
	files {
		"%{prj.location}/src/**.h",
		"%{prj.location}/src/**.cpp",
		"Externals/json/nlohmann_json.natvis",
		"Externals/glm/util/*",
		"Externals/stb/stb.cpp"
	}
	
	includedirs {
		"%{prj.name}/src/",
		"Externals/json/",
		"Externals/glew/include",
		"Externals/ImGui/",
		"Externals/ImGui-premake/",
		"Externals/glm/",
		"Externals/stb/",
		"Externals/optick/include/",
		"Externals/fmt/include/",
		"Externals/entt/",
		"Externals/imgui_entt_entity_editor/",
	}
	
	libdirs {
		"Externals/glew/lib/",
	}

	links {
		"Externals/glew/lib/glew32s.lib",
		"ImGui",
		"opengl32.lib",
	}
		
	defines {
		"PLOP_BUILD",
		"GLEW_STATIC"
	}
