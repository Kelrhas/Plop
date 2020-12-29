workspace "Plop"
	startproject "Sample"
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
	
	-- #import is not compatible with /MP
	filter { "files:**/Debug/Debug.cpp" }
		buildoptions "/MP1"
	filter {}

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

--project "Premake"
--	kind "Utility"
--
--	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
--	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
--
--	files
--	{
--		"%{wks.location}/**premake5.lua"
--	}
--
--	postbuildmessage "Regenerating project files with Premake5!"
--	postbuildcommands
--	{
--		"%{prj.location}bin/premake5 %{_ACTION} --file=\"%{wks.location}premake5.lua\""
--	}

include "Externals/imgui"		-- imgui project
include "Plop/"					-- engine project
include "Sample/"				-- sample project
include "TD/"					-- game project
