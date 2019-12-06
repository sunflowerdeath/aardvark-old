cmake_minimum_required(VERSION 3.10)

# Compiler must be set before "project"
if (ADV_COMPILER_CLANG)
    set(CMAKE_C_COMPILER "/usr/bin/clang")
    set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
endif()

project(AARDVARK LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if(NOT ADV_PLATFORM)
    set(ADV_PLATFORM "linux")
endif()

if(NOT ADV_ARCH)
    set(ADV_ARCH "x86_64")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

# Export compile commands to 'compile_commands.json'
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#-------------------------------------------------------------------------------
# COMPILATION SETTINGS
#-------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS_DEBUG "-g -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Os -g -fno-omit-frame-pointer")

# Release
# -Os - Optimize for size
# -ffunction-sections -fdata-sections 
#	Put data and code in their own sections, so that unused symbols
#	can be removed at link time with --gc-sections
# -Wl,--gc-sections - Linker option to remove unneeded sections
# -Wl,-s - Run strip to reduce size of executable
set(CMAKE_CXX_FLAGS_RELEASE "-Os -ffunction-sections -fdata-sections")
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections -Wl,-s")
    set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections -Wl,-s")
endif()

#-------------------------------------------------------------------------------
# LIBS
#-------------------------------------------------------------------------------

# jsi
set(ADV_JSI_STATIC_LIB ON CACHE BOOL "" FORCE)
set(ADV_JSI_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ADV_JSI_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libs/jsi")
