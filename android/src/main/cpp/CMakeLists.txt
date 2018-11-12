cmake_minimum_required(VERSION 3.4.1)

# set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS}")

# native_app_glue
add_library(native_app_glue STATIC
	${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
)
target_include_directories(native_app_glue INTERFACE
	${ANDROID_NDK}/sources/android/native_app_glue
)

# skia
set(SKIA_DIR ${CMAKE_SOURCE_DIR}/../../../../core/externals/skia)

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

set(SKIA_DEFINES
	#"SK_SAMPLES_FOR_X" # always set for linux, even if there's no X used
	"SK_SUPPORT_GPU=1" # skia_enable_gpu
	"SK_HAS_JPEG_LIBRARY" # skia_use_libjpeg_turbo
	"SK_HAS_PNG_LIBRARY" # skia_use_libpng
	# "SK_SUPPORT_PDF" # skia_enable_pdf
	#"SK_PDF_USE_SFNTLY" # skia_use_sfntly
	#"SK_HAS_WEBP_LIBRARY" # skia_use_libwebp
	"SK_XML" # skia_use_expat
)

add_library(skia STATIC IMPORTED GLOBAL)
set_target_properties(skia PROPERTIES
	IMPORTED_LOCATION "${SKIA_DIR}/lib/android-arm64/libskia.a"
	INTERFACE_INCLUDE_DIRECTORIES "${SKIA_INCLUDE_DIRS}"
	INTERFACE_COMPILE_DEFINITIONS "${SKIA_DEFINES}"
)
# INTERFACE_LINK_LIBRARIES "${SKIA_DEPENDENCIES}"

# now build app's shared lib
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -Wall -Werror")

# Export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set(CMAKE_SHARED_LINKER_FLAGS
	"${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate"
)

add_library(native-activity SHARED main.cpp)
target_link_libraries(native-activity
	android native_app_glue EGL GLESv2 log skia
)
