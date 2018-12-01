cmake_minimum_required(VERSION 3.10)
project(hello LANGUAGES CXX)

# TODO just use -std=cxx-17?
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Export compile commands to 'compile_commands.json'
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Prefer static libraries instead of dynamic ones
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.so")

# set(CMAKE_CXX_FLAGS_DEBUG "-g")

#-------------------------------------------------------------------------------
# RELEASE OPTIMIZATIONS
#
# -Os
#   Optimize for size
# -ffunction-sections -fdata-sections 
#	Put data and code in their own sections, so that unused symbols
#	can be removed at link time with --gc-sections
# -Wl,--gc-sections
#   Linker option to remove unneeded sections
# -s
#   Run strip
#-------------------------------------------------------------------------------
set(CMAKE_CXX_FLAGS_RELEASE
  "-Os -ffunction-sections -fdata-sections -Wl,--gc-sections -s")

#-------------------------------------------------------------------------------
# DEPENDENCIES
#-------------------------------------------------------------------------------

# fmt
add_subdirectory("${CMAKE_BINARY_DIR}/externals/fmt")

# GLFW
# set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
# set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
# set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
# add_subdirectory(${CMAKE_BINARY_DIR}/externals/glfw)

# SDL2
find_package(SDL2 REQUIRED)
add_library(SDL2_LIB STATIC IMPORTED)
set_target_properties(SDL2_LIB PROPERTIES
	IMPORTED_LOCATION ${SDL2_LIBRARIES}
	INTERFACE_INCLUDE_DIRECTORIES ${SDL2_INCLUDE_DIRS}
)

# skia
set(SKIA_DIR "${CMAKE_BINARY_DIR}/externals/skia")

set(SKIA_INCLUDE_DIRS
	${SKIA_DIR}/include/c
	${SKIA_DIR}/include/codec
	${SKIA_DIR}/include/config
	${SKIA_DIR}/include/core
	${SKIA_DIR}/include/effects
	${SKIA_DIR}/include/gpu
	${SKIA_DIR}/include/gpu/gl
	${SKIA_DIR}/include/pathops
	${SKIA_DIR}/include/ports
	${SKIA_DIR}/include/svg
	${SKIA_DIR}/include/utils
	${SKIA_DIR}/include/private
)

## TODO
# https://github.com/zub2/SkiaExample/blob/master/ext/CMakeLists.txt#L134
# this must match what BUILD.gn sets
# if it's wrong it can result in all kinds of funny behavior
set(SKIA_DEFINES
	"SK_SAMPLES_FOR_X" # always set for linux, even if there's no X used
	"SK_SUPPORT_GPU=1" # skia_enable_gpu
	"SK_HAS_JPEG_LIBRARY" # skia_use_libjpeg_turbo
	"SK_HAS_PNG_LIBRARY" # skia_use_libpng
	# "SK_SUPPORT_PDF" # skia_enable_pdf
	"SK_PDF_USE_SFNTLY" # skia_use_sfntly
	"SK_HAS_WEBP_LIBRARY" # skia_use_libwebp
	"SK_XML" # skia_use_expat
)

find_package(OpenGL)
find_library(DL_LIB libdl.so)
find_library(PTHREAD_LIB libpthread.so)
find_library(EXPAT_LIB expat) # skia_use_expat skia_use_system_expat
find_library(JPEG_LIB jpeg) # skia_use_system_libjpeg_turbo
find_library(PNG_LIB png) # skia_use_system_libpng
find_library(Z_LIB z) # ??
find_library(WEBP_LIB webp) # skia_use_libwebp skia_use_system_libwebp
find_library(WEBP_DEMUX_LIB webpdemux)
find_library(WEBP_MUX_LIB webpmux)
find_library(FONTCONFIG_LIB libfontconfig.so) # skia_use_fontconfig
find_library(FREETYPE_LIB freetype) # skia_use_system_freetype2

set(SKIA_DEPENDENCIES
	OpenGL::GL
	${DL_LIB}
	${PTHREAD_LIB}
	${EXPAT_LIB}
	${JPEG_LIB}
	${PNG_LIB}
	${Z_LIB}
	${WEBP_LIB}
	${WEBP_DEMUX_LIB}
	${WEBP_MUX_LIB}
	${FONTCONFIG_LIB}
	${FREETYPE_LIB}
)

# TODO just skia?
add_library(SKIA_LIB STATIC IMPORTED GLOBAL)
set_target_properties(SKIA_LIB PROPERTIES
	IMPORTED_LOCATION "${SKIA_DIR}/lib/linux-x86_64/libskia.a"
	INTERFACE_INCLUDE_DIRECTORIES "${SKIA_INCLUDE_DIRS}"
	INTERFACE_COMPILE_DEFINITIONS "${SKIA_DEFINES}"
	INTERFACE_LINK_LIBRARIES "${SKIA_DEPENDENCIES}"
)

# ICU
set(ICU_DIR ${CMAKE_BINARY_DIR}/externals/icu)

add_library(ICU STATIC IMPORTED)
set_target_properties(ICU PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES ${ICU_DIR}/source/common
	IMPORTED_LOCATION "${ICU_DIR}/build/lib/libicuuc.a"
	INTERFACE_COMPILE_DEFINITIONS
		"UCONFIG_NO_COLLATION=1 UCONFIG_NO_FORMATTING=1 U_STATIC_IMPLEMENTATION=1"
)

add_library(ICU_DATA STATIC IMPORTED)
set_target_properties(ICU_DATA PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES ${ICU_DIR}/source/common
	IMPORTED_LOCATION "${ICU_DIR}/build/lib/libicudata.a"
)

# JavaScriptCore
set(WEBKIT_DIR ${CMAKE_BINARY_DIR}/externals/WebKit)

add_library(WTF STATIC IMPORTED)
set_target_properties(WTF PROPERTIES
	IMPORTED_LOCATION ${WEBKIT_DIR}/lib/libWTF.a
	INTERFACE_INCLUDE_DIRECTORIES "${WEBKIT_DIR}/Source/WTF/wtf"
	INTERFACE_LINK_LIBRARIES "${DL_LIB};ICU;ICU_DATA"
	INTERFACE_COMPILE_DEFINITIONS "ENABLE_INTL=0"
)

add_library(JavaScriptCore STATIC IMPORTED)
set_target_properties(JavaScriptCore PROPERTIES
	IMPORTED_LOCATION ${WEBKIT_DIR}/lib/libJavaScriptCore.a
	INTERFACE_INCLUDE_DIRECTORIES "${WEBKIT_DIR}/include"
	INTERFACE_LINK_LIBRARIES WTF
)

#-------------------------------------------------------------------------------
# PROJECT SETUP
#-------------------------------------------------------------------------------
add_library(aardvark
  src/base_types.cpp
  src/box_constraints.cpp
  src/compositing.cpp
  src/layer_tree.cpp
  src/document.cpp
  src/element.cpp
  src/window.cpp
)
# add_dependencies(hello fmt SKIA_LIB JavaScriptCore)
target_link_libraries(aardvark 
	fmt
	SKIA_LIB
	SDL2_LIB
	JavaScriptCore
)

#-------------------------------------------------------------------------------
# PROJECT SETUP
#-------------------------------------------------------------------------------
add_executable(example src/example.cpp)
add_dependencies(example aardvark)
target_link_libraries(example 
	aardvark
	SDL2_LIB
)

#-------------------------------------------------------------------------------
# PROJECT SETUP
#-------------------------------------------------------------------------------
add_executable(hello src/hello.cpp)
add_dependencies(hello fmt SKIA_LIB JavaScriptCore)
target_link_libraries(hello 
	fmt
	SKIA_LIB
	SDL2_LIB
	JavaScriptCore
)
