#!/bin/bash -e

ICU_DIR=$PWD/externals/icu

CFLAGS="-fPIC -DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_FORMATTING=1 -Oz"
CXXFLAGS="$CFLAGS --std=c++11"

mkdir -p $ICU_DIR/build
cd $ICU_DIR/build
$ICU_DIR/source/configure \
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
    CFLAGS="$CFLAGS" \
    CXXFLAGS="$CXXFLAGS" \
    CC=clang \
    CXX=clang++ \
    --with-data-packaging=static

make -j7

cp $ICU_DIR/build/stubdata/libicudata.a $ICU_DIR/build/lib/


