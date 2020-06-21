#!/bin/bash -e

ICU_DIR=$PWD/externals/icu

# usage: 
# PLATFORM=android ARCH=arm|arm64 ./scripts/build_icu.sh
# PLATFORM=linux ARCH=x86|x86_64 ./scripts/build_icu.sh

COMMON_OPTIONS=" \
    --enable-static=yes \
    --enable-shared=no \
    --enable-extras=no \
    --enable-strict=no \
    --enable-icuio=no \
    --enable-layout=no \
    --enable-layoutex=no \
    --enable-tests=no \
    --enable-samples=no \
    --enable-dyload=no \
    --with-data-packaging=static"
COMMON_CFLAGS="-fPIC -DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_FORMATTING=1 -Oz"
COMMON_CXXFLAGS="$COMMON_CFLAGS --std=c++11"

mkdir -p $ICU_DIR/build-$PLATFORM-$ARCH
cd $ICU_DIR/build-$PLATFORM-$ARCH

if [ "$PLATFORM" = "linux" ]; then
    $ICU_DIR/source/configure \
        $COMMON_OPTIONS \
        CFLAGS="$COMMON_CFLAGS" \
        CXXFLAGS="$COMMON_CXXFLAGS" \
        CC=clang \
        CXX=clang++
fi

if [ "$PLATFORM" = "android" ]; then
    if [ "$ARCH" = "arm64" ]; then
        CROSS_COMPILE_PLATFORM="aarch64-linux-android"
    elif [ "$ARCH" = "arm" ]; then
        CROSS_COMPILE_PLATFORM="armv7a-linux-androideabi"
    fi

    ANDROID_NDK=$NDK_PATH
    ANDROID_API=21
    TOOLCHAIN_DIR=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin
    CC=$TOOLCHAIN_DIR/${CROSS_COMPILE_PLATFORM}${ANDROID_API}-clang
    CXX=$TOOLCHAIN_DIR/${CROSS_COMPILE_PLATFORM}${ANDROID_API}-clang++
    LDFLAGS="-lc -lstdc++"

    $ICU_DIR/source/configure \
        ${COMMON_OPTIONS} \
        --host=$CROSS_COMPILE_PLATFORM \
        --with-cross-build=$ICU_DIR/build-linux-x86_64 \
        CFLAGS="$COMMON_CFLAGS" \
        CXXFLAGS="$COMMON_CXXFLAGS" \
        LDFLAGS="$LDFLAGS" \
        CC=$CC \
        CXX=$CXX \
        AR=$TOOLCHAIN_DIR/$CROSS_COMPILE_PLATFORM-ar \
        RANLIB=$TOOLCHAIN_DIR/$CROSS_COMPILE_PLATFORM-ranlib
fi

make -j7
