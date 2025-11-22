cmake_minimum_required(VERSION 3.10)

# Project name
project(x86 C)

# Output
add_library(x86 STATIC
"main.c"
)

if(ARCH STREQUAL "32")
	message(STATUS "Building ${PROJECT_NAME} for 32-bit")
	target_compile_options(x86 PRIVATE -m32)
elseif(ARCH STREQUAL "64")
	message(STATUS "Building ${PROJECT_NAME} for 64-bit")
	target_compile_options(x86 PRIVATE -m64)
else()
	message(FATAL_ERROR "Unknown architecture for ${PROJECT_NAME}: ${ARCH} (expected 32 or 64)")
endif()

# Compile flags
target_compile_options(x86 PRIVATE -Wall)

set_target_properties(x86 PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/langs"
)
