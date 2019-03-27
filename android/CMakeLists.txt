cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17")

# aardvark
set(ADV_PLATFORM "android")
set(ADV_ARCH "arm64")
set(ADV_BUILD_EXAMPLES OFF)
set(ADV_BUILD_TESTS OFF)
set(ADV_ENABLE_JS ON)
set(ADV_STATIC_LIB OFF)
add_subdirectory("../core" "../core/build-android-arm64")

# # Export ANativeActivity_onCreate(),
# # Refer to: https://github.com/android-ndk/ndk/issues/381.
# set(CMAKE_SHARED_LINKER_FLAGS
    # "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
# add_library(native-activity SHARED src/main/cpp/main2.cpp)
# target_link_libraries(native-activity aardvark)

add_library(aardvark-android SHARED src/main/cpp/main.cpp)
target_link_libraries(aardvark-android aardvark)
