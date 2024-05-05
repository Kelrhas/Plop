cmake_minimum_required(VERSION 3.20)

# Ask for c++20
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 20)

# Remove the ZERO_CHECK vcxproj
set(CMAKE_SUPPRESS_REGENERATION true)

# Set the output folder
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/build)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/build) 
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG 	"${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE 	"${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MASTER 	"${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG	"${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE 	"${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_MASTER 	"${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG	"${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE	"${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_MASTER 	"${CMAKE_CURRENT_SOURCE_DIR}/bin")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

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

###############################################################################
# Configure Plop engine projects

message("\n")
message("===============================================================")
message("Generating project from ${CMAKE_CURRENT_SOURCE_DIR}")
message("===============================================================")

project(PlopGames
		LANGUAGES CXX
		VERSION 0.1)

add_subdirectory(PlopCore)
add_subdirectory(Editor)
add_subdirectory(Sample)


# need to list every game directory available