cmake_minimum_required(VERSION 3.14)
project(lng C)

set(CMAKE_C_STANDARD 11)

option(LNG_NATIVE "Enable native CPU optimizations" OFF)

if(NOT MSVC)
    add_compile_options(-O3 -Wall -Wextra -Werror -fno-finite-math-only)
    add_definitions(-D_GNU_SOURCE)
    if(LNG_NATIVE)
        add_compile_options(-march=native)
    endif()
else()
    add_compile_options(/O2 /W3 /WX)
endif()

set(LNG_SOURCES src/liblng.c)
set(LNG_HEADERS src/lng.h)

add_library(lng_shared SHARED ${LNG_SOURCES})
set_target_properties(lng_shared PROPERTIES OUTPUT_NAME lng PREFIX "lib")
target_include_directories(lng_shared PRIVATE src)

add_library(lng_static STATIC ${LNG_SOURCES})
set_target_properties(lng_static PROPERTIES
    OUTPUT_NAME lng
    PREFIX "lib"
    POSITION_INDEPENDENT_CODE ON
)
target_include_directories(lng_static PRIVATE src)

add_executable(lng_exe src/lng.c ${LNG_SOURCES})
set_target_properties(lng_exe PROPERTIES OUTPUT_NAME lng)
target_include_directories(lng_exe PRIVATE src)

if(UNIX AND NOT ANDROID)
    target_link_libraries(lng_shared pthread m)
    target_link_libraries(lng_static pthread m)
    target_link_libraries(lng_exe pthread m)
elseif(ANDROID)
    target_link_libraries(lng_shared m)
    target_link_libraries(lng_static m)
    target_link_libraries(lng_exe m)
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm|armv7)")
    if(NOT ANDROID)
        target_link_libraries(lng_shared atomic)
        target_link_libraries(lng_static atomic)
        target_link_libraries(lng_exe atomic)
    endif()
endif()
