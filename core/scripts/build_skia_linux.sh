#!/bin/bash -e

SKIA_DIR=$PWD/externals/skia
BUILD_DIR=$SKIA_DIR/build-linux-x86_64

# Skia build configuration:
# is_official_build=true	Use system libs and disable test utilities 
# skia_use_dng_sdk=false	RAW image handling
# skia_use_piex=false       preview extractor, used for RAW
# skia_enable_pdf=false		PDF handling
# skia_use_sfntly=false		lib for working with fonts, optional dep for PDF 
# skia_use_expat=false      XML parsing lib (for SVG and Android fontmanager )
$SKIA_DIR/bin/gn gen --root=$SKIA_DIR $BUILD_DIR --args="\
is_official_build=true \
cc=\"clang\" \
cxx=\"clang++\" \
skia_use_icu=false \
skia_use_dng_sdk=false \
skia_use_piex=false \
skia_use_sfntly=false \
skia_use_libwebp=false \
skia_enable_pdf=false \
skia_use_expat=false"

ninja -C $BUILD_DIR -j7
