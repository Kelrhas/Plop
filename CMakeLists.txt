cmake_minimum_required(VERSION 3.20)

# Ask for c++20
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 20)

# Remove the ZERO_CHECK vcxproj
set(CMAKE_SUPPRESS_REGENERATION true)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

message("\n")
message("===============================================================")
message("Generating project from ${CMAKE_CURRENT_SOURCE_DIR}")
message("===============================================================")

set(ENGINE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
configure_file(DirectorySettings.h.in "${ENGINE_PATH}/DirectorySettings.h")
message("Engine path: ${ENGINE_PATH}")

# Set the configuration targets
set(CMAKE_CONFIGURATION_TYPES Debug;Release;Master)

add_compile_definitions(
	$<$<CONFIG:Debug>:_DEBUG>
	$<$<CONFIG:Release>:_RELEASE>
	$<$<CONFIG:Master>:_MASTER>
	$<$<PLATFORM_ID:Windows>:PLATFORM_WINDOWS>
	$<$<PLATFORM_ID:Linux>:PLATFORM_LINUX>
	GLEW_STATIC
	_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS
)
	
	
# some strange stuff to do with variables/list and genexp ...
set( msvc_warnings "/W4;/WX")
set( other_warnings "-Wall;-Wextra;-Wpedantic;-Werror")
add_compile_options(
	# MSVC warnings
	#$<$<CXX_COMPILER_ID:MSVC>:${msvc_warnings}>
	$<$<CXX_COMPILER_ID:MSVC>:/W4>
	# MSVC Optim per config
	$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/Od>
	$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/Ox>
	$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Master>>:/O2>
	# MSVC misc
	$<$<CXX_COMPILER_ID:MSVC>:/MP> # parallel build
	$<$<CXX_COMPILER_ID:MSVC>:/external:anglebrackets> # include with angle brackets are considered external
	$<$<CXX_COMPILER_ID:MSVC>:/external:W0> # external includes warning
	$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/ZI> # Incremental linking
	
	
	#
	#    CHECK THIS 
	#
	# Other compilers warnings
	#$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:${other_warnings}>
	## Other compilers Optim per config
	#$<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<CONFIG:Debug>>:-O0>
	#$<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<CONFIG:Release>>:-O2>
	#$<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<CONFIG:Master>>:-O3>
)

add_link_options(
	$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/INCREMENTAL> # Incremental linking
)

###############################################################################
# Configure Plop engine projects

project(PlopGames
		LANGUAGES CXX
		VERSION 0.1)

add_subdirectory(PlopCore)
add_subdirectory(Editor)
add_subdirectory(Sample)

# DYNAMIC_TARGETS (depends on folder present at the same level as the engine), DO NOT REMOVE
