cmake_minimum_required(VERSION 3.10)

# Project name
project(assembler C)


# Create an object file target (this will produce one .o file)
add_library(assembler STATIC 
"src/main.c"
"src/argparse/argparse.c"
"src/argparse/strtoXX/strtonnum.c"
"src/argparse/strtoXX/strtonunum.c"
"src/argparse/strtoXX/safe-ctype.c"
)

target_compile_options(assembler PRIVATE -Wall)

# Executable
add_executable(nyasm "src/.c")

if(ARCH STREQUAL "32")
	message(STATUS "Building ${PROJECT_NAME} for 32-bit")
	target_compile_options(assembler PRIVATE -m32)
	target_compile_options(nyasm PRIVATE -m32)
	target_link_options(nyasm PRIVATE -m32)
	add_definitions(-DARCH=${ARCH})
elseif(ARCH STREQUAL "64")
	message(STATUS "Building ${PROJECT_NAME} for 64-bit")
	target_compile_options(assembler PRIVATE -m64)
	target_compile_options(nyasm PRIVATE -m64)
	target_link_options(nyasm PRIVATE -m64)
	add_definitions(-DARCH=${ARCH})
else()
	message(FATAL_ERROR "Unknown architecture for ${PROJECT_NAME}: ${ARCH} (expected 32 or 64)")
endif()


# Add languages here (don't forget to link)
add_subdirectory(src/x86)

# Add links here
target_link_libraries(nyasm PRIVATE assembler x86)

target_link_options(nyasm PRIVATE -T ${CMAKE_SOURCE_DIR}/src/linker.ld)

set_target_properties(nyasm PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/"
)