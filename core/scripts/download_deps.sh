#!/bin/bash -e

# This script must be run from root of the project
ROOT=$PWD

if [ -z $DEP ]; then
  $DEP = "all"
fi

mkdir -p $ROOT/downloads
cd $ROOT/downloads

if [ "$DEP" = "boost" ] || [ "$DEP" = "all" ]; then
	echo "download boost@1.71.0"
	# curl flag "-L" to follow redirects, "-#" to display progressbar
	curl "https://dl.bintray.com/boostorg/release/1.71.0/source/boost_1_71_0.tar.gz" -L -# \
		> boost_1_71_0.tar.gz
fi

if [ "$DEP" = "catch2" ] || [ "$DEP" = "all" ]; then
	echo "download Catch2@2.5.0"
	curl "https://github.com/catchorg/Catch2/releases/download/v2.5.0/catch.hpp" -L -# > catch.hpp
fi

if [ "$DEP" = "fmt" ] || [ "$DEP" = "all" ]; then
	echo "clone fmt@7.0.1"
	curl "https://github.com/fmtlib/fmt/releases/download/7.0.1/fmt-7.0.1.zip" -L -# > fmt-7.0.1.zip
fi

if [ "$DEP" = "glfw" ] || [ "$DEP" = "all" ]; then
	echo "download GLFW@3.2.1"
	curl "https://github.com/glfw/glfw/releases/download/3.2.1/glfw-3.2.1.zip" -L -# > glfw-3.2.1.zip
fi

if [ "$DEP" = "icu" ] || [ "$DEP" = "all" ]; then
	echo "download ICU@58.2"
	curl "http://download.icu-project.org/files/icu4c/58.2/icu4c-58_2-src.tgz" -L -# > \
		icu4c-58_2-src.tgz
	echo "download ICU data"
	curl "http://download.icu-project.org/files/icu4c/58.2/icu4c-58_2-data.zip" -L -# > \
		icu4c-58_2-data.zip
fi

if [ "$DEP" = "nod" ] || [ "$DEP" = "all" ]; then
	echo "download nod@0.5.0"
	curl "https://github.com/fr00b0/nod/archive/v0.5.0.zip" -L -# > nod-0.5.0.zip
fi

if [ "$DEP" = "skia" ] || [ "$DEP" = "all" ]; then
	echo "download skia@m71"
	curl https://github.com/google/skia/archive/chrome/m71.zip -L -# > skia-chrome-m71.zip
	# git clone https://github.com/google/skia.git -b chrome/m71 --depth 1
	# git clone https://skia.googlesource.com/skia.git -b chrome/m71 --depth 1
fi

if [ "$DEP" = "skia-last" ] || [ "$DEP" = "all" ]; then
	echo "download skia@m85"
	curl https://github.com/google/skia/archive/chrome/m85.zip -L -# > skia-chrome-m85.zip
fi

if [ "$DEP" = "spdlog" ] || [ "$DEP" = "all" ]; then
	echo "download spdlog@1.7.0"
	curl "https://github.com/gabime/spdlog/archive/v1.7.0.zip" -L -# > spdlog-1.7.0.zip
fi

if [ "$DEP" = "webkitgtk" ] || [ "$DEP" = "all" ]; then
	echo "download webkitgtk@2.23.2"
	curl "https://webkitgtk.org/releases/webkitgtk-2.23.2.tar.xz" -# > webkitgtk-2.23.2.tar.xz
fi
