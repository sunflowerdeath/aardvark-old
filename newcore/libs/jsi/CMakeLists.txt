cmake_minimum_required(VERSION 3.10)

project(ADV_JSI LANGUAGES C CXX)

#-------------------------------------------------------------------------------
# TARGETS
#-------------------------------------------------------------------------------
# Library
if (ADV_JSI_SHARED)
    set(ADV_JSI_LIB_TYPE SHARED)
else()
    set(ADV_JSI_LIB_TYPE STATIC)
endif()

add_library(aardvark_jsi ${ADV_JSI_LIB_TYPE}
    src/check.cpp
    src/jsi.cpp
    src/mappers.cpp
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
if(ADV_JSI_TESTS)
    add_executable(aadrvark_jsi_tests
        tests/main.cpp
        tests/check_test.cpp
        tests/jsi_test.cpp
        tests/mappers_test.cpp
        tests/idl_test.cpp
        tests/idl/function.cpp
        generated/enum.cpp
        generated/struct.cpp
        generated/class.cpp
        generated/function.cpp
        generated/callback.cpp
    )
    target_link_libraries(aadrvark_jsi_tests Catch2 aardvark_jsi)
endif()
