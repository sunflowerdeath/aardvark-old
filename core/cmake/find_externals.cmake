find_library(DL_LIB libdl.so)
find_package(Threads)

set(ADV_EXERNALS_DIR "${CMAKE_CURRENT_LIST_DIR}/../externals")

# boost
set(BOOST_ROOT "${ADV_EXERNALS_DIR}/boost")
find_package(Boost 1.71.0 REQUIRED)
add_library(boost INTERFACE)
set_target_properties(boost PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
    
# Catch2
add_library(Catch2 INTERFACE)
set_target_properties(Catch2 PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${ADV_EXERNALS_DIR}/Catch2/include)
    
# expected
add_library(expected INTERFACE)
set_target_properties(expected PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${ADV_EXERNALS_DIR}/expected/include)

# fmt
add_subdirectory("${ADV_EXERNALS_DIR}/fmt"
    ${CMAKE_CURRENT_BINARY_DIR}/fmt)

# glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
find_package(X11 REQUIRED)
add_subdirectory(${ADV_EXERNALS_DIR}/glfw)

# ICU
set(ICU_DIR ${ADV_EXERNALS_DIR}/icu)
set(ICU_LIB_DIR ${ICU_DIR}/build-${ADV_PLATFORM}-${ADV_ARCH}/lib)

add_library(ICU STATIC IMPORTED)
set_target_properties(ICU PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${ICU_DIR}/source/common
    IMPORTED_LOCATION "${ICU_LIB_DIR}/libicuuc.a"
    INTERFACE_COMPILE_DEFINITIONS
    "UCONFIG_NO_COLLATION=1 UCONFIG_NO_FORMATTING=1 U_STATIC_IMPLEMENTATION=1"
)

add_library(ICU_DATA STATIC IMPORTED)
set_target_properties(ICU_DATA PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${ICU_DIR}/source/common
    IMPORTED_LOCATION "${ICU_LIB_DIR}/libicudata.a"
)

# JavaScriptCore
set(WEBKIT_DIR "${ADV_EXERNALS_DIR}/WebKit")
set(WEBKIT_LIB_DIR "${WEBKIT_DIR}/build-${ADV_PLATFORM}-${ADV_ARCH}/lib")

add_library(WTF STATIC IMPORTED)
set_target_properties(WTF PROPERTIES
    IMPORTED_LOCATION ${WEBKIT_LIB_DIR}/libWTF.a
    INTERFACE_INCLUDE_DIRECTORIES "${WEBKIT_DIR}/Source/WTF/wtf"
    INTERFACE_LINK_LIBRARIES "${CMAKE_THREAD_LIBS_INIT};${DL_LIB};ICU;ICU_DATA"
    INTERFACE_COMPILE_DEFINITIONS "ENABLE_INTL=0")

add_library(JavaScriptCore STATIC IMPORTED)
set_target_properties(JavaScriptCore PROPERTIES
    IMPORTED_LOCATION ${WEBKIT_LIB_DIR}/libJavaScriptCore.a
    INTERFACE_INCLUDE_DIRECTORIES "${WEBKIT_DIR}/include"
    INTERFACE_LINK_LIBRARIES WTF)

# json
add_library(nlohmann_json INTERFACE)
set_target_properties(nlohmann_json PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES
    ${ADV_EXERNALS_DIR}/nlohmann_json/include)

# nod
add_library(nod INTERFACE)
set_target_properties(nod PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES
    ${ADV_EXERNALS_DIR}/nod/include)

# quickjs
set(QUICKJS_DIR "${ADV_EXERNALS_DIR}/quickjs")

add_library(quickjs STATIC IMPORTED)
set_target_properties(quickjs PROPERTIES
    IMPORTED_LOCATION "${QUICKJS_DIR}/libquickjs.a"
    INTERFACE_INCLUDE_DIRECTORIES "${QUICKJS_DIR}/include"
    INTERFACE_LINK_LIBRARIES "${DL_LIB}")

# skia
set(SKIA_DIR "${ADV_EXERNALS_DIR}/skia")

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
	${SKIA_DIR}/include/private)

# This must match what skia's BUILD.gn sets
set(SKIA_DEFINES
	"SK_SUPPORT_GPU=1"      # skia_enable_gpu
	"SK_HAS_JPEG_LIBRARY"   # skia_use_libjpeg_turbo
	"SK_HAS_PNG_LIBRARY"    # skia_use_libpng
	# "SK_SUPPORT_PDF"      # skia_enable_pdf
	# "SK_PDF_USE_SFNTLY"   # skia_use_sfntly
	# "SK_HAS_WEBP_LIBRARY" # skia_use_libwebp
	"SK_XML"                # skia_use_expat
)

# find_library(WEBP_LIB webp.a) # skia_use_libwebp skia_use_system_libwebp
# find_library(WEBP_DEMUX_LIB webpdemux.a)
# find_library(WEBP_MUX_LIB webpmux.a)
	# ${WEBP_LIB}
	# ${WEBP_DEMUX_LIB}
	# ${WEBP_MUX_LIB}

set(SKIA_DEPS ${DL_LIB} ${CMAKE_THREAD_LIBS_INIT})

if (ADV_PLATFORM STREQUAL "linux")
    set(SKIA_DEFINES ${SKIA_DEFINES} "SK_SAMPLES_FOR_X") # always set for linux

    cmake_policy(SET CMP0072 NEW)       # prefer GLVND libs to legacy GL lib
    find_package(OpenGL REQUIRED)
    find_library(Z_LIB z)               # "skia_use_system_zlib"
    find_library(FONTCONFIG_LIB libfontconfig.so) # skia_use_fontconfig
    find_library(FREETYPE_LIB freetype) # skia_use_system_freetype2
    find_library(JPEG_LIB jpeg)         # skia_use_system_libjpeg_turbo
    find_library(PNG_LIB png)           # skia_use_system_libpng

    set(SKIA_DEPS 
        ${SKIA_DEPS}
        OpenGL::OpenGL
		OpenGL::GLX
        ${Z_LIB}
        ${FONTCONFIG_LIB}
        ${FREETYPE_LIB}
        ${JPEG_LIB}
        ${PNG_LIB})
endif()
if (ADV_PLATFORM STREQUAL "android")
    set(SKIA_DEFINES ${SKIA_DEFINES} "SK_BUILD_FOR_ANDROID")
    set(SKIA_DEPS  ${SKIA_DEPS} "EGL")
endif()

add_library(skia STATIC IMPORTED GLOBAL)
set_target_properties(skia PROPERTIES
	IMPORTED_LOCATION "${SKIA_DIR}/build-${ADV_PLATFORM}-${ADV_ARCH}/libskia.a"
	INTERFACE_INCLUDE_DIRECTORIES "${SKIA_INCLUDE_DIRS}"
	INTERFACE_COMPILE_DEFINITIONS "${SKIA_DEFINES}"
	INTERFACE_LINK_LIBRARIES "${SKIA_DEPS}")

# spdlog
set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "")
add_subdirectory(${ADV_EXERNALS_DIR}/spdlog)