
project "ImGui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
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
		"%{prj.location}/imgui_impl_opengl3.cpp",
		"%{prj.location}/imgui_impl_opengl3.h",
		"%{prj.location}/imgui_custom.cpp",
		"%{prj.location}/imgui_custom.h",
		"%{prj.location}/imstb_rectpack.h",
		"%{prj.location}/imstb_textedit.h",
		"%{prj.location}/imstb_truetype.h",
		"%{prj.location}/misc/cpp/imgui_stdlib.cpp",
		"%{prj.location}/misc/cpp/imgui_stdlib.h",
		"%{prj.location}/misc/natvis/imgui.natvis",

		-- ImGuizmo
		"%{prj.location}/../imguizmo/ImCurveEdit.cpp",
		"%{prj.location}/../imguizmo/ImCurveEdit.h",
		"%{prj.location}/../imguizmo/ImGradient.cpp",
		"%{prj.location}/../imguizmo/ImGradient.h",
		"%{prj.location}/../imguizmo/ImGuizmo.cpp",
		"%{prj.location}/../imguizmo/ImGuizmo.h",
		"%{prj.location}/../imguizmo/ImSequencer.cpp",
		"%{prj.location}/../imguizmo/ImSequencer.h",
		"%{prj.location}/../imguizmo/ImZoomSlider.h",
	}
	
	filter "system:Windows"
		files
		{
			"%{prj.location}/imgui_impl_win32.cpp",
			"%{prj.location}/imgui_impl_win32.h",
		}
	filter {}
	
	includedirs
	{
		"%{prj.location}",
		"%{prj.location}/../glm", -- for glm::vec2 <-> ImVec2 conversion in imconfig
		"../glew/include",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"IMGUI_IMPL_OPENGL_LOADER_GLEW"
	}
	
		