#!/bin/bash -e

SKIA_DIR=$PWD/externals/skia
BUILD_DIR=$SKIA_DIR/lib/linux-x86-64

# Skia build configuration:
# is_official_build=true	Use system libs and disable test utilities 
# skia_use_dng_sdk=false	RAW image handling
# skia_use_sfntly=false		PDF handling depedency
# skia_enable_pdf=false		PDF handling
$SKIA_DIR/bin/gn gen --root=$SKIA_DIR $BUILD_DIR --args="\
is_official_build=true \
cc=\"clang\" \
cxx=\"clang++\" \
skia_use_icu=false \
skia_use_dng_sdk=false \
skia_use_sfntly=false \
skia_enable_pdf=false \
skia_use_libwebp=false"

ninja -C $BUILD_DIR -j7
