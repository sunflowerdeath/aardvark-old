#!/bin/bash -e

SKIA_DIR=$PWD/externals/skia

$SKIA_DIR/bin/gn gen build-arm --args="
ndk_api=21 \
ndk=\"/Users/jie/android/android-ndk-r16b\" \
target_cpu=\"arm\" \
target_os=\"android\" \
cc=\"clang\" \
cxx=\"clang++\" \
is_component_build=true \
is_official_build=true \
is_debug=false \
skia_use_icu=false \
skia_use_zlib=false \
skia_use_libjpeg_turbo=false \
skia_use_libwebp=false \
skia_use_piex=false \
skia_use_system_expat=false \
skia_use_system_freetype2=false \
skia_use_system_libpng=false \
"

# is_component_build=true - shared library