cmake_minimum_required(VERSION 3.10)

project(ADV_JS LANGUAGES C CXX)

#-------------------------------------------------------------------------------
# TARGETS
#-------------------------------------------------------------------------------
# Library
if (ADV_JS_SHARED)
    set(ADV_JS_LIB_TYPE SHARED)
else()
    set(ADV_JS_LIB_TYPE STATIC)
endif()

add_library(aardvark_js ${ADV_JS_LIB_TYPE}
    generated/error_location_api.cpp
    src/api/animation_frame.cpp
    src/module_loader.cpp
    src/api/element.cpp
    src/api/transform.cpp
)

if(ADV_PLATFORM STREQUAL "linux")
    target_sources(aardvark_js PRIVATE 
        generated/api.cpp
        src/api/timeout.cpp
        src/host.cpp)
endif()

if(ADV_PLATFORM STREQUAL "android")
    target_sources(aardvark_js PRIVATE
        generated/android_api.cpp
        src/android_host.cpp
        src/android_jni.cpp)
endif()

make_includable(
    "${ADV_JS_DIR}/packages/sourcemap/build/getOriginalLocation.js"
    "${CMAKE_CURRENT_SOURCE_DIR}/generated/getOriginalLocation.js")

target_include_directories(aardvark_js PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/aardvark_js
    ${CMAKE_CURRENT_SOURCE_DIR}/generated
)

target_include_directories(aardvark_js PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(aardvark_js aardvark_ui aardvark_jsi)

# Examples
if(ADV_JS_EXAMPLES)
    add_executable(adv_js_example
        examples/example.cpp
    )
    target_link_libraries(adv_js_example aardvark_ui aardvark_js)
endif()

# Tests
if(ADV_JS_TESTS)
    add_executable(adv_js_tests
        tests/main.cpp
        tests/module_loader_test.cpp
    )
    target_link_libraries(adv_js_tests aardvark_js Catch2)
endif()
