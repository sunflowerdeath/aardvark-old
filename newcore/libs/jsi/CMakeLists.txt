cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

project(ADV_JSI LANGUAGES C CXX)

#-------------------------------------------------------------------------------
# COMPILATION SETTINGS
#-------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS_DEBUG "-g -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Os -g -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -ffunction-sections -fdata-sections")

#-------------------------------------------------------------------------------
# TARGETS
#-------------------------------------------------------------------------------
include("${CMAKE_CURRENT_SOURCE_DIR}/../../externals/find.cmake")

# Library
if (ADV_JSI_STATIC_LIB)
    set(ADV_JSI_LIB_TYPE STATIC)
else()
    set(ADV_JSI_LIB_TYPE SHARED)
endif()

add_library(aardvark_jsi ${ADV_JSI_LIB_TYPE}
    # src/check.cpp
    src/jsi.cpp
    # src/mappers.cpp
)

target_include_directories(aardvark_jsi PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/aardvark_jsi)

target_include_directories(aardvark_jsi INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(aardvark_jsi fmt expected)

if (ADV_JSI_JSC)
    target_sources(aardvark_jsi PRIVATE src/jsc.cpp)
    target_link_libraries(aardvark_jsi JavaScriptCore)
    target_compile_definitions(aardvark_jsi INTERFACE ADV_JSI_JSC=1)
endif()

if (ADV_JSI_QJS)
    target_sources(aardvark_jsi PRIVATE src/qjs.cpp)
    target_link_libraries(aardvark_jsi quickjs)
    target_compile_definitions(aardvark_jsi INTERFACE ADV_JSI_QJS=1)
endif()

# Tests
if(ADV_JSI_BUILD_TESTS)
    add_executable(tests
        tests/main.cpp
        # tests/check_test.cpp
        tests/jsi_test.cpp
        # tests/mappers_test.cpp
    )
    target_link_libraries(tests Catch2 aardvark_jsi)
endif()
