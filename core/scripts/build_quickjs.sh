#!/bin/bash -e

QJS_DIR=$PWD/externals/quickjs

if [ "$PLATFORM" = "android" ]; then
    ANDROID_NDK=$NDK_PATH
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

BUILD_DIR=$QJS_DIR/build-$PLATFORM-$ARCH
rm -rf $BUILD_DIR
mkdir $BUILD_DIR
cd $BUILD_DIR
cmake $CMAKE_ARGS ..
make -j4
