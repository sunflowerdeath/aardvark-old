#!/bin/bash -e

# Prepares downloaded external dependencies to build

# This script must be run from core directory
ROOT=$PWD

if [ -z $DEP ]; then
  $DEP = "all"
fi

mkdir -p $ROOT/externals

if [ "$DEP" = "boost" ] || [ "$DEP" = "all" ]; then
	echo "boost: extract source"
	tar -xf $ROOT/downloads/boost_1_71_0.tar.gz -C $ROOT/externals
	mv $ROOT/externals/boost_1_71_0 $ROOT/externals/boost
fi

if [ "$DEP" = "catch2" ] || [ "$DEP" = "all" ]; then
	CATCH_DIR=$ROOT/externals/Catch2
	echo "Catch2: copy source"
	mkdir -p $CATCH_DIR/include/Catch2
	cp $ROOT/downloads/catch.hpp $CATCH_DIR/include/Catch2/catch.hpp
fi

if [ "$DEP" = "fmt" ] || [ "$DEP" = "all" ]; then
	echo "fmt: extract source"
	unzip -q $ROOT/downloads/fmt-5.3.0.zip -d $ROOT/externals
	mv $ROOT/externals/fmt-5.3.0 $ROOT/externals/fmt
fi

if [ "$DEP" = "glfw" ] || [ "$DEP" = "all" ]; then
	echo "GLFW: extract source"
	unzip -q $ROOT/downloads/glfw-3.2.1.zip -d $ROOT/externals
	mv $ROOT/externals/glfw-3.2.1 $ROOT/externals/glfw
fi

if [ "$DEP" = "icu" ] || [ "$DEP" = "all" ]; then
	ICU_DIR=$ROOT/externals/icu
	echo "ICU: extract source"
	tar -xf $ROOT/downloads/icu4c-58_2-src.tgz -C $ROOT/externals
	echo "ICU: extract data"
	# unzip flag "-q" is quiet, "-o" is overwrite
	unzip  -o -q $ROOT/downloads/icu4c-58_2-data.zip -d $ICU_DIR/source
	echo "ICU: patch source"
	patch -d $ICU_DIR -p2 < $ROOT/scripts/icu.patch
	rm -f $ICU_DIR/source/data/in/icudt58l.dat
	rm -f $ICU_DIR/source/data/mappings/ucmcore.mk
	rm -f $ICU_DIR/source/data/mappings/ucmebcdic.mk
	rm -f $ICU_DIR/source/data/mappings/ucmfiles.mk
	echo "ICU: copy include headers"
	mkdir -p $ICU_DIR/include
	cp -r $ICU_DIR/source/common/unicode $ICU_DIR/include
	cp -r $ICU_DIR/source/i18n/unicode $ICU_DIR/include
	cp -r $ICU_DIR/source/layoutex/layout $ICU_DIR/include/layout
fi

if [ "$DEP" = "nod" ] || [ "$DEP" = "all" ]; then
	echo "nod: extract source"
	unzip -q $ROOT/downloads/nod-0.5.0.zip -d $ROOT/externals
	mv $ROOT/externals/nod-0.5.0 $ROOT/externals/nod
fi

# skia
if [ "$DEP" = "skia" ] || [ "$DEP" = "all" ]; then
	SKIA_DIR=$ROOT/externals/skia
	echo "Skia: copy source"
	cp $ROOT/downloads/skia $SKIA_DIR -r
	# Patch disables unneeded third-party deps to reduce download size
	echo "Skia: patch source"
	patch -d $SKIA_DIR -p3 < $ROOT/scripts/skia.patch
	echo "Skia: download third party deps"
	$SKIA_DIR/tools/git-sync-deps
fi

if [ "$DEP" = "spdlog" ] || [ "$DEP" = "all" ]; then
	echo "spdlog: extract source"
	unzip -q $ROOT/downloads/spdlog-1.4.2.zip -d $ROOT/externals
	mv $ROOT/externals/spdlog-1.4.2 $ROOT/externals/spdlog
fi

if [ "$DEP" = "webkitgtk" ] || [ "$DEP" = "all" ]; then
	WEBKIT_DIR=$ROOT/externals/WebKit
	echo "WebKit: extract source"
	tar -xf $ROOT/downloads/webkitgtk-2.23.2.tar.xz -C $ROOT/externals
	mv $ROOT/externals/webkitgtk-2.23.2 $WEBKIT_DIR
	# Patch for webkit allows to use custom version of ICU library and
	# disables ICU collation
	echo "WebKit: patch source"
	patch -d $WEBKIT_DIR -p3 < $ROOT/scripts/WebKit.patch
	echo "WebKit: copy include headers"
	mkdir -p $WEBKIT_DIR/include/JavaScriptCore
	cp -r $WEBKIT_DIR/Source/JavaScriptCore/API $WEBKIT_DIR/include/JavaScriptCore
fi
