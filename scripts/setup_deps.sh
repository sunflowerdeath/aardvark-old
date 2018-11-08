#!/bin/bash -e

# This script must be run from root of the project
ROOT=$PWD

mkdir -p $ROOT/externals

echo "WebKit: copy source"
cp $ROOT/downloads/WebKit $ROOT/externals/WebKit -r

echo "WebKit: patch files"
patch -d $ROOT/externals/WebKit -p3 < $ROOT/scripts/WebKit.patch

echo "WebKit: copy include headers"
mkdir -p $ROOT/externals/WebKit/include/JavaScriptCore
cp -r $ROOT/externals/WebKit/Source/JavaScriptCore/API $ROOT/externals/WebKit/include/JavaScriptCore

echo "Skia: copy source"
cp $ROOT/downloads/skia $ROOT/externals/skia -r

echo "Skia: fetch gn"
$ROOT/externals/skia/bin/fetch-gn

echo "ICU: extract source"
tar -xf $ROOT/downloads/icu4c-58_2-src.tgz -C $ROOT/externals
