#!/bin/bash -e

SKIA_DIR=$PWD/externals/skia
BUILD_DIR=$SKIA_DIR/build-android-arm64

$SKIA_DIR/bin/gn gen $BUILD_DIR --root=$SKIA_DIR --args="\
is_official_build=true \
target_os=\"android\" \
target_cpu=\"arm64\" \
skia_use_icu=false \
skia_use_dng_sdk=false \
skia_use_sfntly=false \
skia_use_piex=false \
skia_enable_pdf=false \
skia_use_libwebp=false \
skia_use_system_expat=false \
skia_use_system_freetype2=false \
skia_use_system_libjpeg_turbo=false \
skia_use_system_libpng=false \
skia_use_system_zlib=false \
ndk=\"${ANDROID_HOME}/ndk/21.0.6113669/\" \
ndk_api=21"

ninja -C $BUILD_DIR -j7
