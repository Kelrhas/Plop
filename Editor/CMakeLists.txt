cmake_minimum_required(VERSION 3.16)

add_executable(Editor)
target_include_directories(Editor PRIVATE src)
target_link_directories(Editor PRIVATE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG})
target_link_libraries(Editor PRIVATE PlopCore)
target_link_libraries(Editor PRIVATE
	$<$<CONFIG:Debug>:PlopCore_d>
	$<$<CONFIG:Release>:PlopCore_r>
#	$<$<CONFIG:Master>:PlopCore>
)

set_target_properties(Editor PROPERTIES
					FOLDER "Editor"
					VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
					DEBUG_POSTFIX "_d"
					RELEASE_POSTFIX "_r"
)

add_subdirectory(src)

#source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" FILES
#	${SOURCE_FILES_PARENT}
#	${HEADER_FILES_PARENT}
#)