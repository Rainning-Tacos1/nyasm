cmake_minimum_required(VERSION 3.10)

# Project name
project(_6502 C)

add_library(${PROJECT_NAME} STATIC
    "main.c"
)

if(ARCH STREQUAL "32")
    message(STATUS "Building ${PROJECT_NAME} for 32-bit")
    target_compile_options(${PROJECT_NAME} PRIVATE -m32)
elseif(ARCH STREQUAL "64")
    message(STATUS "Building ${PROJECT_NAME} for 64-bit")
    target_compile_options(${PROJECT_NAME} PRIVATE -m64)
else()
    message(FATAL_ERROR "Unknown architecture for ${PROJECT_NAME}: ${ARCH} (expected 32 or 64)")
endif()

# Compile flags
target_compile_options(${PROJECT_NAME} PRIVATE -Wall)

set_target_properties(${PROJECT_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out"
)
