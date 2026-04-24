cmake_minimum_required(VERSION 3.14)
project(prol C)

set(CMAKE_C_STANDARD 11)

# Output binary to the root directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Performance optimizations
option(PROL_NATIVE "Enable native optimizations" OFF)

if(NOT MSVC)
    add_compile_options(-O3 -fno-finite-math-only)
    add_definitions(-D_GNU_SOURCE)
    
    if(PROL_NATIVE)
        add_compile_options(-march=native)
    endif()
else()
    add_compile_options(/O2)
endif()

# Main executable
add_executable(prol 
    prol.c 
    libprol.c
)

# Include directories
target_include_directories(prol PRIVATE .)

# System libraries
if(UNIX)
    target_link_libraries(prol pthread m)
endif()
