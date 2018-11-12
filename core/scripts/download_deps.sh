#!/bin/bash -e

# This script must be run from root of the project
ROOT=$PWD

mkdir -p $ROOT/downloads
cd $ROOT/downloads

# fmt@5.2.1
git clone https://github.com/fmtlib/fmt -b 5.2.1 --depth 1

# skia@m71
git clone https://skia.googlesource.com/skia.git -b chrome/m71 --depth 1
./skia/bin/fetch-gn

# webkitgtk@2.22.3
mkdir WebKit
WEBKIT_VERSION=2.22.2
SVN_URL=https://svn.webkit.org/repository/webkit/releases/WebKitGTK/webkit-$WEBKIT_VERSION
svn export $SVN_URL/Source WebKit/Source
svn export $SVN_URL/Tools WebKit/Tools
svn export $SVN_URL/CMakeLists.txt WebKit/CMakeLists.txt
# sudo apt install libicu-dev python ruby bison flex cmake build-essential ninja-build git gperf

# ICU@58.2
mkdir icu
curl "http://download.icu-project.org/files/icu4c/58.2/icu4c-58_2-src.tgz" > icu4c-58_2-src.tgz