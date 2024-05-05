cmake_minimum_required(VERSION 3.16)

message("Adding Sample as a standalone")
add_executable(Sample)
target_include_directories(Sample PRIVATE src)
target_include_directories(Sample
    PRIVATE
    "$<TARGET_PROPERTY:PlopCore,INTERFACE_INCLUDE_DIRECTORIES>"
)
target_link_directories(Sample PRIVATE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG})
target_link_libraries(Sample PRIVATE PlopCore)
set_target_properties(Sample PROPERTIES
					VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
					DEBUG_POSTFIX "_d"
					RELEASE_POSTFIX "_r"
					)
					




message("Adding Sample as a library")
add_library(SampleLib SHARED)
target_include_directories(SampleLib PRIVATE src)
target_include_directories(SampleLib
    PRIVATE
    "$<TARGET_PROPERTY:PlopCore,INTERFACE_INCLUDE_DIRECTORIES>"
)
target_link_directories(SampleLib PRIVATE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG})
target_link_libraries(SampleLib PRIVATE PlopCore)
set_target_properties(SampleLib PROPERTIES
					FOLDER "Editor"
					DEBUG_POSTFIX "_d"
					RELEASE_POSTFIX "_r"
					)

#target_compile_definitions(SampleLib PRIVATE LIBRARY_EXPORT)


# Include sources for both target
add_subdirectory(src)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" FILES
	${SOURCE_FILES_PARENT}
	${HEADER_FILES_PARENT}
)