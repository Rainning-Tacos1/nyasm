cmake_minimum_required(VERSION 3.10)

# Project name
project(assembler C)


# Create an object file target (this will produce one .o file)
add_library(assembler STATIC 
"src/main.c"
"src/argparse/argparse.c"
"src/argparse/strtoXX/strtonum.c"
"src/argparse/strtoXX/strtounum.c"
"src/argparse/strtoXX/safe-ctype.c"
)

include_directories(${CMAKE_SOURCE_DIR}/include)
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


# Add languages to build individually or with the assembler
add_subdirectory(langs/x86)


# Link the language to include it in the assembler
target_link_libraries(nyasm PRIVATE assembler x86)

target_link_options(nyasm PRIVATE -T ${CMAKE_SOURCE_DIR}/src/linker.ld -Wl,--undefined=__asm_lang_anchor)

set_target_properties(nyasm PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/"
)