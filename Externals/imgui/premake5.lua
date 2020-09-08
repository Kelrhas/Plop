
project "ImGui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	location "."
	
	files
	{
		"%{prj.location}/imconfig.h",
		"%{prj.location}/imgui.cpp",
		"%{prj.location}/imgui.h",
		"%{prj.location}/imgui_demo.cpp",
		"%{prj.location}/imgui_draw.cpp",
		"%{prj.location}/imgui_internal.h",
		"%{prj.location}/imgui_widgets.cpp",
		"%{prj.location}/imstb_rectpack.h",
		"%{prj.location}/imstb_textedit.h",
		"%{prj.location}/imstb_truetype.h",
		"%{prj.location}/misc/cpp/imgui_stdlib.cpp",
		"%{prj.location}/misc/cpp/imgui_stdlib.h",
		"%{prj.location}/misc/natvis/imgui.natvis"
	}
	
	includedirs
	{
		"%{prj.location}",
		"%{prj.location}/../glm", -- for glm::vec2 <-> ImVec2 conversion in imconfig
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}
	
		