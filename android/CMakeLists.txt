cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17")

# native_app_glue
add_library(native_app_glue STATIC
	${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
target_include_directories(native_app_glue INTERFACE
	${ANDROID_NDK}/sources/android/native_app_glue)

# aardvark
set(ADV_PLATFORM "android")
set(ADV_ARCH "arm64")
set(ADV_BUILD_EXAMPLES OFF)
set(ADV_BUILD_TESTS OFF)
set(ADV_ENABLE_JS OFF)
set(ADV_STATIC_LIB OFF)
add_subdirectory("../core" "../core/build-android-arm64")

# Export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set(CMAKE_SHARED_LINKER_FLAGS
	"${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
add_library(native-activity SHARED 
    src/main/cpp/main.cpp)
target_link_libraries(native-activity
	android native_app_glue EGL GLESv2 log aardvark)
