cmake_minimum_required(VERSION 3.10)

project(ADV_JSI LANGUAGES C CXX)

#-------------------------------------------------------------------------------
# COMPILATION SETTINGS
#-------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS_DEBUG "-g -fno-omit-frame-pointer -fsanitize=address")
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
    src/jsi.cpp
    src/jsc.cpp
)

target_include_directories(aardvark_jsi PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/aardvark_jsi)

target_include_directories(aardvark_jsi INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include)
    
target_link_libraries(aardvark_jsi JavaScriptCore)

# Tests
if(ADV_JSI_BUILD_TESTS)
    add_executable(tests
        tests/main.cpp
        tests/jsc_test.cpp
    )
    target_link_libraries(tests Catch2 aardvark_jsi)
endif()
