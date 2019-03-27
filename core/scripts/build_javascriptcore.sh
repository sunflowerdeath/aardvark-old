#!/bin/bash -e

ICU_DIR=$PWD/externals/icu
WEBKIT_DIR=$PWD/externals/WebKit

# Compiler flags:
# -DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_FORMATTING=1 - Disabled ICU features
# -DU_STATIC_IMPLEMENTATION - Required when ICU is built as static library
C_FLAGS=" \
-DENABLE_INTL=0 \
-DUCONFIG_NO_COLLATION=1 \
-DUCONFIG_NO_FORMATTING=1 \
-DU_STATIC_IMPLEMENTATION=1 \
-ffunction-sections -fdata-sections \
"

# Compiler flags argument contain spaces and CMake does not support quotes,
# so they can be passed only using export variables
export CFLAGS="$C_FLAGS"
export CXXFLAGS="$C_FLAGS"

# CMake args:
# -DPORT=JSCOnly - Build only JavaScriptCore
# -DENABLE_STATIC_JSC - Build as static library
# -DUSE_SYSTEM_MALLOC=ON - Do not use webkit memory allocator to reduce size
# -DCUSTOM_ICU_INCLUDE_DIR, -DCUSTOM_ICU_LIBRARY_DIR - Paths to custom ICU library

# -DUSE_THIN_ARCHIVES=OFF \ // ?
CMAKE_ARGS="\
-DCMAKE_BUILD_TYPE=Release \
-DPORT=JSCOnly \
-DENABLE_STATIC_JSC=ON \
-DENABLE_TOOLS=OFF \
-DENABLE_API_TESTS=OFF \
-DUSE_SYSTEM_MALLOC=ON \
-DENABLE_INTL=OFF \
-DCUSTOM_ICU_INCLUDE_DIR=$ICU_DIR/include \
-DCUSTOM_ICU_LIBRARY_DIR=$ICU_DIR/build-linux-x86_64/lib \
-DENABLE_JIT=ON \
-DENABLE_FTL_JIT=OFF \
-DENABLE_DFG_JIT=ON \
-DENABLE_WEBASSEMBLY=OFF \
-DENABLE_XSLT=OFF \
"

if [ "$PLATFORM" = "android" ]; then
    ANDROID_NDK=$ANDROID_HOME/ndk-bundle
    ANDROID_API=21
    TOOLCHAIN_DIR=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64

    if [ "$ARCH" = "arm64" ]; then
        CROSS_COMPILE_PLATFORM="aarch64-linux-android"
        ARCH_NAME="aarch64"
    elif [ "$ARCH" = "arm" ]; then
        CROSS_COMPILE_PLATFORM="armv7a-linux-androideabi"
        ARCH_NAME="armv7"
    fi

    CMAKE_ARGS="$CMAKE_ARGS \
        -DCMAKE_SYSTEM_NAME=Android \
        -DANDROID_NDK=$ANDROID_NDK \
        -DANDROID_PLATFORM=$ANDROID_API \
        -DANDROID_ABI=arm64-v8a \
        -DANDROID_STL=c++_static \
        -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
    "
fi

BUILD_DIR=$WEBKIT_DIR/build-$PLATFORM-$ARCH
rm -rf $BUILD_DIR
mkdir $BUILD_DIR
cd $BUILD_DIR
cmake $CMAKE_ARGS ..
make -j7
