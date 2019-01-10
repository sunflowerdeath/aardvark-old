#!/bin/bash -e

ICU_DIR=$PWD/externals/icu
WEBKIT_DIR=$PWD/externals/WebKit

# Compiler flags:
# -DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_FORMATTING=1 - Disabled ICU features
# -DU_STATIC_IMPLEMENTATION - Required when ICU is built as static library
# -DU_SHOW_CPLUSPLUS_API=0 - Not sure why TODO
C_FLAGS=" \
-DENABLE_INTL=0 \
-DUCONFIG_NO_COLLATION=1 \
-DUCONFIG_NO_FORMATTING=1 \
-DU_STATIC_IMPLEMENTATION=1 \
-DU_SHOW_CPLUSPLUS_API=0 \
-ffunction-sections -fdata-sections
"

# Compiler flags argument contain spaces and CMake does not support quotes,
# so they can be passed only using export variables
export CFLAGS="$C_FLAGS"
export CXXFLAGS="$C_FLAGS"

# CMake args:
# -DPORT=JSCOnly - Build only JavaScriptCore
# -DENABLE_STATIC_JSC - Build as static library
# -DUSE_SYSTEM_MALLOC=ON - Do not use webkit memory allocator to reduce size
# -DCUSTOM_ICU_INCLUDE_DIR, -DCUSTOM_ICU_LIBRARY_DIR - Paths to custom ICU library
CMAKE_ARGS="\
-DCMAKE_BUILD_TYPE=Release \
-DPORT=JSCOnly \
-DENABLE_STATIC_JSC=ON \
-DENABLE_TOOLS=OFF \
-DENABLE_API_TESTS=OFF \
-DUSE_SYSTEM_MALLOC=ON \
-DENABLE_INTL=OFF \
-DCUSTOM_ICU_INCLUDE_DIR=$ICU_DIR/include \
-DCUSTOM_ICU_LIBRARY_DIR=$ICU_DIR/build/lib \
-DENABLE_FTL_JIT=OFF \
-DENABLE_DFG_JIT=ON \
-DENABLE_WEBASSEMBLY=OFF \
-DENABLE_JIT=ON \
"

cd $WEBKIT_DIR
cmake . $CMAKE_ARGS
make -j7
