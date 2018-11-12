#!/bin/bash -e

SKIA_DIR=$PWD/externals/skia
LIB_DIR=$SKIA_DIR/lib/android-arm64

$SKIA_DIR/bin/gn gen $LIB_DIR --root=$SKIA_DIR --args="
is_official_build=true \
skia_enable_tools=false \
target_os=\"android\" \
target_cpu=\"arm64\" \
skia_use_icu=false \
skia_use_sfntly=false \
skia_use_piex=false \
skia_use_libwebp=false \
skia_use_system_expat=false \
skia_use_system_freetype2=false \
skia_use_system_libjpeg_turbo=false \
skia_use_system_libpng=false \
skia_use_system_zlib=false \
ndk=\"${ANDROID_HOME}/ndk-bundle\" \
ndk_api=21"

ninja -C $LIB_DIR -j3