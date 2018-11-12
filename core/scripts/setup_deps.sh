#!/bin/bash -e

# Prepare downloaded external dependencies to build

# This script must be run from core directory
ROOT=$PWD

mkdir -p $ROOT/externals

$WEBKIT_DIR=$ROOT/externals/WebKit
echo "WebKit: copy source"
cp $ROOT/downloads/WebKit $WEBKIT_DIR -r
# Patch for webkit allows to use custom version of ICU library and disables ICU collation
echo "WebKit: patch files"
patch -d $WEBKIT_DIR -p3 < $ROOT/scripts/WebKit.patch
echo "WebKit: copy include headers"
mkdir -p $WEBKIT_DIR/include/JavaScriptCore
cp -r $WEBKIT_DIR/Source/JavaScriptCore/API $ROOT/externals/WebKit/include/JavaScriptCore

$SKIA_DIR=$ROOT/externals/skia
echo "Skia: copy source"
cp $ROOT/downloads/skia $SKIA_DIR -r
# Patch disables unneeded third-party deps to reduce download size
echo "Skia: patch deps"
patch -d $SKIA_DIR -p3 < $ROOT/scripts/skia.patch
echo "Skia: download gn"
$SKIA_DIR/bin/fetch-gn
echo "Skia: download third party deps"
$SKIA_DIR/git-sync-deps

echo "ICU: extract source"
tar -xf $ROOT/downloads/icu4c-58_2-src.tgz -C $ROOT/externals
