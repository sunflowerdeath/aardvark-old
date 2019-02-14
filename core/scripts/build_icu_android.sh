#!/bin/bash -e

ICU_DIR=$PWD/externals/icu

ARCH=android-arm64
ANDROID_NDK=$ANDROID_HOME/ndk-bundle
TOOLCHAIN_DIR=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin
CROSS_COMPILE_PLATFORM=aarch64-linux-android
ANDROID_API=21

CFLAGS="-fPIC -DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_FORMATTING=1 -Oz"
CXXFLAGS="$CFLAGS --std=c++11"
LDFLAGS="-lc -lstdc++"

CC=$TOOLCHAIN_DIR/${CROSS_COMPILE_PLATFORM}${ANDROID_API}-clang
CXX=$TOOLCHAIN_DIR/${CROSS_COMPILE_PLATFORM}${ANDROID_API}-clang++

mkdir -p $ICU_DIR/build-${ARCH}
cd $ICU_DIR/build-${ARCH}
$ICU_DIR/source/configure \
    --host=$CROSS_COMPILE_PLATFORM \
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
    --with-cross-build=$ICU_DIR/-build \
    CFLAGS="$CFLAGS" \
    CXXFLAGS="$CXXFLAGS" \
    LDFLAGS="$LDFLAGS" \
    CC=$CC \
    CXX=$CXX \
    AR=$TOOLCHAIN_DIR/$CROSS_COMPILE_PLATFORM-ar \
    RINLIB=$TOOLCHAIN_DIR/$CROSS_COMPILE_PLATFORM-ranlib \
    --with-data-packaging=static

make -j7
