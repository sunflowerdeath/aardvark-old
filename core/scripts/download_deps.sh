#!/bin/bash -e

# This script must be run from root of the project
ROOT=$PWD

mkdir -p $ROOT/downloads
cd $ROOT/downloads

# fmt@5.2.1
# echo "clone fmt@5.2.1"
# git clone https://github.com/fmtlib/fmt -b 5.2.1 --depth 1

# boost@1.71.0
echo "download boost@1.71.0"
curl "https://dl.bintray.com/boostorg/release/1.71.0/source/boost_1_71_0.tar.gz" -L -# > boost_1_71_0_rc2.tar.gz

# skia@m71
echo "clone skia@m71"
# git clone https://github.com/google/skia.git -b chrome/m71 --depth 1
git clone https://skia.googlesource.com/skia.git -b chrome/m71 --depth 1

# webkitgtk@2.23.2
echo "download webkitgtk"
curl "https://webkitgtk.org/releases/webkitgtk-2.23.2.tar.xz" -# > webkitgtk-2.23.2.tar.xz

# ICU@58.2
echo "download ICU@58.2"
curl "http://download.icu-project.org/files/icu4c/58.2/icu4c-58_2-src.tgz" -# > icu4c-58_2-src.tgz
echo "download ICU data"
curl "http://download.icu-project.org/files/icu4c/58.2/icu4c-58_2-data.zip" -# > icu4c-58_2-data.zip

# GLFW@3.2.1
echo "download GLFW@3.2.1"
# curl flag "-L" to follow redirects, "-#" to display progressbar
curl "https://github.com/glfw/glfw/releases/download/3.2.1/glfw-3.2.1.zip" -L -# > glfw-3.2.1.zip

# Catch2
echo "download Catch2"
curl "https://github.com/catchorg/Catch2/releases/download/v2.5.0/catch.hpp" -L -# > catch.hpp
