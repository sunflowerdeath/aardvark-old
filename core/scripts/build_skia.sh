#!/bin/bash -e

SKIA_DIR=$PWD/externals/skia
BUILD_DIR=$SKIA_DIR/build-$PLATFORM-$ARCH

# Skia build configuration:
# is_official_build=true	    Use system libs and disable test utilities 
# skia_use_dng_sdk=false	RAW image handling
# skia_use_piex=false       preview extractor, used for RAW
# skia_enable_pdf=false		PDF handling
# skia_use_sfntly=false		lib for working with fonts, optional dep for PDF 
BUILD_FLAGS="\
is_official_build=true \
skia_use_icu=false \
skia_use_dng_sdk=false \
skia_use_sfntly=false \
skia_use_piex=false \
skia_enable_pdf=false \
skia_use_libwebp_decode=false \
skia_use_libwebp_encode=false \
skia_enable_skparagraph=false \
skia_enable_skottie=false \
"

if [ "$PLATFORM" = "android" ]; then
    # TODO fix ndk path
    BUILD_FLAGS="$BUILD_FLAGS \
        target_os=\"android\" \
        target_cpu=\"arm64\" \
        ndk=\"${ANDROID_HOME}/ndk/21.0.6113669/\" \
        ndk_api=21 \
        skia_use_system_expat=false \
        skia_use_system_freetype2=false \
        skia_use_system_libjpeg_turbo=false \
        skia_use_system_libpng=false \
        skia_use_system_zlib=false \
    "
fi

if [ "$PLATFORM" = "linux" ]; then
    # skia_use_expat=false  XML parsing lib (for SVG and Android fontmanager)
    BUILD_FLAGS="$BUILD_FLAGS skia_use_expat=false"
fi

if [ "$PLATFORM" = "macos" ]; then
    BUILD_FLAGS="$BUILD_FLAGS \
        skia_use_system_libpng=false \
        skia_use_system_libjpeg_turbo=false \
        target_cpu=\"arm64\" \
    "
fi

$SKIA_DIR/bin/gn gen --root=$SKIA_DIR $BUILD_DIR --args="$BUILD_FLAGS"

ninja -C $BUILD_DIR -j 8
