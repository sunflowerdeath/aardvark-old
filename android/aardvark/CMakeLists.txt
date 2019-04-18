cmake_minimum_required(VERSION 3.10)

set(ADV_PLATFORM "android")
set(ADV_ARCH "arm64")
set(ADV_BUILD_EXAMPLES OFF)
set(ADV_BUILD_TESTS OFF)
set(ADV_ENABLE_JS OFF)
set(ADV_STATIC_LIB OFF)
add_subdirectory("../../core" "../../core/build-android-arm64")
