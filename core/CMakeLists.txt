cmake_minimum_required(VERSION 3.10)

# Compiler must be set before "project"
if (ADV_COMPILER_CLANG)
    set(CMAKE_C_COMPILER "/usr/bin/clang")
    set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
endif()

project(AARDVARK LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

#-------------------------------------------------------------------------------
# OPTIONS
#-------------------------------------------------------------------------------
option(ADV_ENABLE_JS "Enable javascript bindings" OFF)
option(ADV_BUILD_EXAMPLES "Build examples" OFF)
option(ADV_BUILD_TESTS "Build tests" OFF)
option(ADV_STATIC_LIB "Build as static library" OFF)

if(NOT ADV_PLATFORM)
    set(ADV_PLATFORM "linux")
endif()

if(NOT ADV_ARCH)
    set(ADV_ARCH "x86_64")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

# Export compile commands to 'compile_commands.json'
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#-------------------------------------------------------------------------------
# COMPILATION SETTINGS
#-------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "-g -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Os -g -fno-omit-frame-pointer")

# Release
# -Os - Optimize for size
# -ffunction-sections -fdata-sections 
#	Put data and code in their own sections, so that unused symbols
#	can be removed at link time with --gc-sections
# -Wl,--gc-sections - Linker option to remove unneeded sections
# -Wl,-s - Run strip to reduce size of executable
set(CMAKE_CXX_FLAGS_RELEASE "-Os -ffunction-sections -fdata-sections")
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections -Wl,-s")
    set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections -Wl,-s")
endif()

#-------------------------------------------------------------------------------
# DEPENDENCIES
#-------------------------------------------------------------------------------
# fmt
# add_subdirectory("${CMAKE_SOURCE_DIR}/externals/fmt")

#json
add_library(nlohmann_json INTERFACE)
set_target_properties(nlohmann_json PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/nlohmann_json/include)

# skia
set(SKIA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/externals/skia")

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

find_library(DL_LIB libdl.so)
find_package(Threads)

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
    find_package(OpenGL)
    find_library(Z_LIB z)               # "skia_use_system_zlib"
    find_library(FONTCONFIG_LIB libfontconfig.so) # skia_use_fontconfig
    find_library(EXPAT_LIB expat)       # skia_use_expat skia_use_system_expat
    find_library(FREETYPE_LIB freetype) # skia_use_system_freetype2
    find_library(JPEG_LIB jpeg)         # skia_use_system_libjpeg_turbo
    find_library(PNG_LIB png)           # skia_use_system_libpng

    set(SKIA_DEPS 
        ${SKIA_DEPS}
        OpenGL::GL
        ${Z_LIB}
        ${EXPAT_LIB}
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
	IMPORTED_LOCATION "${SKIA_DIR}/lib/${ADV_PLATFORM}-${ADV_ARCH}/libskia.a"
	INTERFACE_INCLUDE_DIRECTORIES "${SKIA_INCLUDE_DIRS}"
	INTERFACE_COMPILE_DEFINITIONS "${SKIA_DEFINES}"
	INTERFACE_LINK_LIBRARIES "${SKIA_DEPS}")

# ICU
set(ICU_DIR ${CMAKE_CURRENT_SOURCE_DIR}/externals/icu)
set(ICU_LIB_LOCATION ${ICU_DIR}/build-${ADV_PLATFORM}-${ADV_ARCH}/lib)

add_library(ICU STATIC IMPORTED)
set_target_properties(ICU PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${ICU_DIR}/source/common
    IMPORTED_LOCATION "${ICU_LIB_LOCATION}/libicuuc.a"
    INTERFACE_COMPILE_DEFINITIONS
    "UCONFIG_NO_COLLATION=1 UCONFIG_NO_FORMATTING=1 U_STATIC_IMPLEMENTATION=1")

add_library(ICU_DATA STATIC IMPORTED)
set_target_properties(ICU_DATA PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${ICU_DIR}/source/common
    IMPORTED_LOCATION "${ICU_LIB_LOCATION}/libicudata.a")

if(ADV_ENABLE_JS)
    # JavaScriptCore
    set(WEBKIT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/externals/WebKit")
    set(WEBKIT_LIB_DIR "${WEBKIT_DIR}/build-${ADV_PLATFORM}-${ADV_ARCH}/lib")

    add_library(WTF STATIC IMPORTED)
    set_target_properties(WTF PROPERTIES
        IMPORTED_LOCATION ${WEBKIT_LIB_DIR}/libWTF.a
        INTERFACE_INCLUDE_DIRECTORIES "${WEBKIT_DIR}/Source/WTF/wtf"
        INTERFACE_LINK_LIBRARIES "${DL_LIB};ICU;ICU_DATA"
        INTERFACE_COMPILE_DEFINITIONS "ENABLE_INTL=0")

    add_library(JavaScriptCore STATIC IMPORTED)
    set_target_properties(JavaScriptCore PROPERTIES
        IMPORTED_LOCATION ${WEBKIT_LIB_DIR}/libJavaScriptCore.a
        INTERFACE_INCLUDE_DIRECTORIES "${WEBKIT_DIR}/include"
        INTERFACE_LINK_LIBRARIES WTF)
endif()

if(ADV_PLATFORM STREQUAL "linux")
    # glfw
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    find_package(X11 REQUIRED)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/externals/glfw)
endif()

if(ADV_PLATFORM STREQUAL "android")
    # native_app_glue
    add_library(native_app_glue STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
    target_include_directories(native_app_glue INTERFACE
        ${ANDROID_NDK}/sources/android/native_app_glue)
endif()

if(ADV_BUILD_TESTS)
    # Catch2
    add_library(Catch2 INTERFACE)
    set_target_properties(Catch2 PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/externals/Catch2/include)
endif()

#-------------------------------------------------------------------------------
# LIBRARY
#-------------------------------------------------------------------------------
if (ADV_STATIC_LIB)
    set(ADV_LIB_TYPE STATIC)
else()
    set(ADV_LIB_TYPE SHARED)
endif()

add_library(aardvark ${ADV_LIB_TYPE}
    src/base_types.cpp
    src/box_constraints.cpp
    src/hit_tester.cpp
    src/layer.cpp
    src/layer_tree.cpp
    src/document.cpp
    src/element.cpp
    src/paint_cache.cpp
    src/responder.cpp
    src/inline_layout/span.cpp
    src/inline_layout/decoration_span.cpp
    src/inline_layout/line_metrics.cpp
    src/inline_layout/text_span.cpp
    src/inline_layout/utils.cpp
    src/elements/align.cpp
    src/elements/background.cpp
    src/elements/border.cpp
    src/elements/center.cpp
    src/elements/clip.cpp
    src/elements/fixed_size.cpp
    src/elements/gesture_responder.cpp
    src/elements/layer.cpp
    src/elements/paragraph.cpp
    src/elements/stack.cpp
    src/elements/text.cpp)

# TODO include dir
target_include_directories(aardvark INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(aardvark skia ICU ICU_DATA nlohmann_json)

if(ADV_ENABLE_JS)
    target_sources(aardvark PRIVATE
        src/js/bindings_host.cpp
        src/js/base_types_bindings.cpp
        src/js/document_bindings.cpp
        src/js/elements_bindings.cpp)
    if (ADV_PLATFORM STREQUAL "linux")
        target_sources(aardvark PRIVATE
            src/js/desktop_app_bindings.cpp
            src/js/desktop_window_bindings.cpp)
    endif()
    target_link_libraries(aardvark JavaScriptCore)
endif()

if(ADV_PLATFORM STREQUAL "linux")
    target_compile_definitions(aardvark PRIVATE "ADV_PLATFORM_DESKTOP=1")
    target_sources(aardvark PRIVATE
        src/platforms/desktop/desktop_window.cpp
        src/platforms/desktop/desktop_app.cpp
        src/utils/files_utils.cpp)
    target_link_libraries(aardvark glfw ${X11_LIBRARIES} stdc++fs)
endif()

if(ADV_PLATFORM STREQUAL "android")
    target_compile_definitions(aardvark PRIVATE "ADV_PLATFORM_ANDROID=1")
    target_sources(aardvark PRIVATE
        src/platforms/android/android_app.cpp
        src/platforms/android/android_jni.cpp
        src/platforms/android/android_binary_channel.cpp)
    # src/platforms/android/android_texture_layer.cpp
    target_link_libraries(aardvark android EGL GLESv2 log)
endif()

if(ADV_BUILD_EXAMPLES)
    # add_executable(align_example src/examples/align_example.cpp)
    # target_link_libraries(align_example aardvark)

    # add_executable(border_example src/examples/border_example.cpp)
    # target_link_libraries(border_example aardvark)

    # add_executable(clip_example src/examples/clip_example.cpp)
    # target_link_libraries(clip_example aardvark)

    # add_executable(paint_cache_example src/examples/paint_cache.cpp)
    # target_link_libraries(paint_cache_example aardvark)

    # add_executable(app_example src/examples/app_example.cpp)
    # target_link_libraries(app_example aardvark)

    # add_executable(layer_example src/examples/layer_example.cpp)
    # target_link_libraries(layer_example aardvark)

    # add_executable(paragraph_example src/examples/paragraph_example.cpp)
    # target_link_libraries(paragraph_example aardvark)

    if(ADV_ENABLE_JS)
        add_executable(jsc_example src/examples/jsc_example2.cpp)
        target_link_libraries(jsc_example aardvark)
        configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/src/examples/src.js" 
            "${CMAKE_CURRENT_BINARY_DIR}/bin/src.js"
            COPYONLY)
    endif()
endif()

if(ADV_BUILD_TESTS)
    add_executable(tests
        src/tests/index.cpp
        src/tests/base_types_test.cpp
        src/tests/box_constraints_test.cpp
        src/tests/js_objects_index.cpp
        src/tests/responder_test.cpp
        src/tests/align_test.cpp
        src/tests/text_span_test.cpp)
    target_link_libraries(tests Catch2 aardvark)
endif()
