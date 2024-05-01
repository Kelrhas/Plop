cmake_minimum_required(VERSION 3.16)

set(SOURCE_FILES
		"SampleApp.cpp"
		"SampleComponents.cpp"
		"SampleLayer.cpp"
		"SampleLevel.cpp"
)

set(HEADER_FILES
		"SampleApp.h"
		"SampleComponents.h"
		"SampleLayer.h"
		"SampleLevel.h"
)

# List all source files (and headers for IDE)
target_sources(Sample PRIVATE
	${SOURCE_FILES}
	${HEADER_FILES}
)
target_sources(SampleLib PRIVATE
	${SOURCE_FILES}
	${HEADER_FILES}
)

# Make the source list available for parent, to be used in source_group
foreach(src ${HEADER_FILES})
    list(APPEND TMP "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
endforeach()
set(HEADER_FILES_PARENT ${TMP} PARENT_SCOPE)

set(TMP "")
foreach(src ${SOURCE_FILES})
    list(APPEND TMP "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
endforeach()
set(SOURCE_FILES_PARENT ${TMP} PARENT_SCOPE)
