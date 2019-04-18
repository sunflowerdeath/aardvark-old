cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17")

# aardvark
set(ADV_PLATFORM "android")
set(ADV_ARCH "arm64")
set(ADV_BUILD_EXAMPLES OFF)
set(ADV_BUILD_TESTS OFF)
set(ADV_ENABLE_JS ON)
set(ADV_STATIC_LIB OFF)
add_subdirectory("../../core" "../../core/build-android-arm64")

add_library(aardvark-example SHARED src/main/cpp/main.cpp)
target_link_libraries(aardvark-example aardvark log)
