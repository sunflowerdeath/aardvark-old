cmake_minimum_required(VERSION 3.10)

# Compiler must be set before "project"
if (ADV_COMPILER_CLANG)
    # set(CMAKE_C_COMPILER "/usr/bin/clang")
    set(CMAKE_C_COMPILER "/opt/homebrew/opt/llvm/bin/clang")
    set(CMAKE_CXX_COMPILER "${CMAKE_C_COMPILER}++")
endif()

project(AARDVARK LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

if(NOT ADV_PLATFORM)
    set(ADV_PLATFORM "linux")
endif()

if(NOT ADV_ARCH)
    set(ADV_ARCH "x86_64")
endif()

# compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# configure_file(
    # "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json"
    # "${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json"
    # COPYONLY)

#-------------------------------------------------------------------------------
# COMPILATION SETTINGS
#-------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS_DEBUG "-g -fno-omit-frame-pointer")
if (ADV_ASAN)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
endif()
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Os -g -fno-omit-frame-pointer")

# Release
# -Os - Optimize for size
# -ffunction-sections -fdata-sections 
#     Put data and code in their own sections, so that unused symbols
#     can be removed at link time with --gc-sections
# -Wl,--gc-sections - Linker option to remove unneeded sections
# -Wl,-dead_strip - Same but on macos
# -Wl,-s - Run strip to reduce size of executable
set(CMAKE_CXX_FLAGS_RELEASE "-Os -ffunction-sections -fdata-sections")
if(ADV_LTO)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(ADV_PLATFORM STREQUAL "macos")
        set(CMAKE_EXE_LINKER_FLAGS
            "${CMAKE_EXE_LINKER_FLAGS} -Wl,-dead_strip -Wl,-s")
    else()
        set(CMAKE_EXE_LINKER_FLAGS
            "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections -Wl,-s")
    endif()
    set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections -Wl,-s")
endif()

#-------------------------------------------------------------------------------
# LIBS
#-------------------------------------------------------------------------------
set(ADV_JS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../js")

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/find_externals.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/make_includable.cmake")

# ui
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libs/ui")

# jsi
# set(ADV_JSI_EXAMPLES OFF)
# set(ADV_JSI_TESTS OFF)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libs/jsi")

# js
# set(ADV_JS_EXAMPLES ON)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libs/js")
