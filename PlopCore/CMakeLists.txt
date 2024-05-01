cmake_minimum_required(VERSION 3.16)

# Copy a file by replacing the cmake variables (@VAR@)
#configure_file(src/Version.in ${CMAKE_CURRENT_SOURCE_DIR}/src/Version.h)

# Plop as a library
add_library(PlopCore STATIC)
add_subdirectory(src)
target_include_directories(PlopCore PUBLIC src)

set_target_properties(PlopCore PROPERTIES
					DEBUG_POSTFIX "_d"
					RELEASE_POSTFIX "_r"
)

# glm
find_package(glm REQUIRED)
target_link_libraries(PlopCore PUBLIC glm::glm)

# json
find_package(nlohmann_json REQUIRED)
target_link_libraries(PlopCore PUBLIC nlohmann_json::nlohmann_json)

# EnTT
set(ENTT_INCLUDE_NATVIS)
find_package(EnTT REQUIRED)
target_link_libraries(PlopCore PUBLIC EnTT::EnTT)

# fmt
find_package(fmt REQUIRED)
target_link_libraries(PlopCore PUBLIC fmt::fmt-header-only)

# Optick
find_package(Optick REQUIRED)
target_link_libraries(PlopCore PUBLIC Optick::OptickCore)

# Dear ImGui
set(IMGUI_FILES
	"Externals/imgui/imconfig.h"
	"Externals/imgui/imgui.cpp"
	"Externals/imgui/imgui.h"
    "Externals/imgui/imgui_demo.cpp"
    "Externals/imgui/imgui_draw.cpp"
	# Those files are included in ImGuiPlatform.cpp
    #"Externals/imgui/imgui_impl_opengl3.cpp"
    #"Externals/imgui/imgui_impl_win32.cpp"
    "Externals/imgui/imgui_impl_opengl3.h"
    "Externals/imgui/imgui_impl_win32.h"	
    "Externals/imgui/imgui_internal.h"
    "Externals/imgui/imgui_widgets.cpp"
	"Externals/imgui/imstb_rectpack.h"
	"Externals/imgui/imstb_textedit.h"
	"Externals/imgui/imstb_truetype.h"
	"Externals/imgui/misc/cpp/imgui_stdlib.h"
	"Externals/imgui/misc/cpp/imgui_stdlib.cpp"
	"Externals/imgui/misc/natvis/imgui.natvis"
	
	"Externals/imguizmo/imguizmo.cpp"
	"Externals/imguizmo/imguizmo.h"
)
target_sources(PlopCore PRIVATE ${IMGUI_FILES})
source_group( "ImGui" FILES ${IMGUI_FILES})
target_include_directories(PlopCore PUBLIC "Externals/imgui")
target_include_directories(PlopCore PRIVATE "Externals/imguizmo")

# OpenAL
find_package(OpenAL REQUIRED)
target_link_libraries(PlopCore PUBLIC OpenAL::OpenAL)

# GLEW with cmake
set(GLEW_USE_STATIC_LIBS ON)
#find_package(Glew REQUIRED)
#target_link_libraries(PlopCore PUBLIC GLEW::GLEW)

# GLEW lib
#find_package(GLEW REQUIRED)
target_include_directories(PlopCore PUBLIC "Externals/glew/include")
target_link_libraries(PlopCore PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Externals/glew/lib/glew32s.lib")
target_link_libraries(PlopCore PUBLIC "OpenGL32.lib")
#add_compile_definitions(GLEW_STATIC)

# STB
set(STB_FILES
	"Externals/stb/stb.cpp"
	"Externals/stb/stb_image.h"
)
target_sources(PlopCore PRIVATE ${STB_FILES})
source_group( "STB" FILES ${STB_FILES})
target_include_directories(PlopCore PRIVATE "Externals/stb")

# Proper hierarchy inside VS solution
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" FILES
	${SOURCE_FILES_PARENT}
	${HEADER_FILES_PARENT}
)