cmake_minimum_required(VERSION 3.10)
project(aardvark_android)

set(ADV_PLATFORM "android")
set(ADV_ARCH "arm64")
set(ADV_JSI_QJS ON)
set(ADV_JS_SHARED ON)
add_subdirectory("../../core" "../../core/build-android-arm64")
