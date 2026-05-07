cmake_minimum_required(VERSION 3.10)

# Project name
project(nyasm C)


### Executables/Libs

# Static lib for the core
add_library(core STATIC
"src/core/ast.c"
"src/core/variables.c"
"src/core/core.c"
"src/core/parser.c"
"src/core/lexer.c"
"src/core/ctype.c"
)

# Static lib for implementations
add_library(impl STATIC
"src/log/log.c"
"src/memory/memory.c"
"src/file/file.c"
"src/unicode/unicode.c"
"src/unicode/utf8/utf8proc_data.c"
"src/unicode/utf8/utf8proc.c"
)

# Frontend + final executable
add_executable(nyasm 
"src/main.c"
"src/argparse/argparse.c"
"src/argparse/strtoXX/strtonum.c"
"src/argparse/strtoXX/strtounum.c"
"src/argparse/strtoXX/safe-ctype.c"
)


### Include dirs

include_directories(${CMAKE_SOURCE_DIR}/include)


### Compile flags

target_compile_options(nyasm PRIVATE -g -Wall)
target_compile_options(core PRIVATE  -g -Wall)
target_compile_options(impl PRIVATE  -g -Wall)

if(ARCH STREQUAL "32")
    message(STATUS "Building ${PROJECT_NAME} for 32-bit")
    target_compile_options(core PRIVATE -m32)
    target_compile_options(impl PRIVATE -m32)
    target_compile_options(nyasm PRIVATE -m32 -D_FILE_OFFSET_BITS=64)
    target_link_options(nyasm PRIVATE -m32)
    add_definitions(-DARCH=${ARCH})
elseif(ARCH STREQUAL "64")
    message(STATUS "Building ${PROJECT_NAME} for 64-bit")
    target_compile_options(core PRIVATE -m64)
    target_compile_options(impl PRIVATE -m64)
    target_compile_options(nyasm PRIVATE -m64)
    target_link_options(nyasm PRIVATE -m64)
    add_definitions(-DARCH=${ARCH})
else()
    message(FATAL_ERROR "Unknown architecture for ${PROJECT_NAME}: ${ARCH} (expected 32 or 64)")
endif()

### Custom languages

# Add languages to build individually or with the assembler
add_subdirectory(langs/x86)
add_subdirectory(langs/6502)

### Links

# Link the language to include in the assembler
target_link_libraries(nyasm PRIVATE
    core
    impl
    m
    "-Wl,--whole-archive" x86 _6502 "-Wl,--no-whole-archive"
)

target_link_options(nyasm PRIVATE -T ${CMAKE_SOURCE_DIR}/src/linker.ld)


### Outputs

# Output the core to ./out
set_target_properties(core PROPERTIES
ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out"
)

# Output the implementation to ./out
set_target_properties(impl PROPERTIES
ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out"
)

# Output the final executable to ./out & ./
set_target_properties(nyasm PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/"
)