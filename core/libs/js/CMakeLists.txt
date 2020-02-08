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
    generated/api.cpp
    src/host.cpp
    src/module_loader.cpp
)

target_include_directories(aardvark_js PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/aardvark_js
    ${CMAKE_CURRENT_SOURCE_DIR}/generated
)

target_include_directories(aardvark_js INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(aardvark_js aardvark_ui aardvark_jsi)

# Tests
if(ADV_JS_EXAMPLES)
    add_executable(adv_js_example
        examples/example.cpp
    )
    target_link_libraries(adv_js_example aardvark_js)
endif()
