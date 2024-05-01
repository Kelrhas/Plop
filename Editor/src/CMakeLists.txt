cmake_minimum_required(VERSION 3.16)

set(SOURCE_FILES
		"Editor.cpp"
)

set(HEADER_FILES
		"Editor.h"
)

# List all source files (and headers for IDE)
target_sources(Editor PRIVATE
	${SOURCE_FILES}
	${HEADER_FILES}
)

# Make the source list available for parent, to be used in source_group
#foreach(src ${HEADER_FILES})
#    list(APPEND TMP "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
#endforeach()
#set(HEADER_FILES_PARENT ${TMP} PARENT_SCOPE)
#
#set(TMP "")
#foreach(src ${SOURCE_FILES})
#    list(APPEND TMP "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
#endforeach()
#set(SOURCE_FILES_PARENT ${TMP} PARENT_SCOPE)

# Add include directory
# target_include_directories(Plop PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
