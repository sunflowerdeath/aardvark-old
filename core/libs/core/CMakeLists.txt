cmake_minimum_required(VERSION 3.10)

project(ADV_CORE LANGUAGES C CXX)

#-------------------------------------------------------------------------------
# TARGETS
#-------------------------------------------------------------------------------
# Library
if (ADV_CORE_SHARED)
    set(ADV_CORE_LIB_TYPE SHARED)
else()
    set(ADV_CORE_LIB_TYPE STATIC)
endif()

add_library(aardvark_core ${ADV_CORE_LIB_TYPE}
    src/base_types.cpp
    src/box_constraints.cpp
    src/layer.cpp
    src/layer_tree.cpp
    src/document.cpp
    src/element.cpp
    src/paint_cache.cpp
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
    src/elements/flex.cpp
    src/elements/layer.cpp
    src/elements/padding.cpp
    src/elements/paragraph.cpp
    src/elements/responder.cpp
    src/elements/sized.cpp
    src/elements/scroll.cpp
    src/elements/stack.cpp
    src/elements/text.cpp
    src/elements/translate.cpp
    src/pointer_events/hit_tester.cpp
    src/pointer_events/pointer_event_manager.cpp
    src/utils/event_loop.cpp
    src/utils/websocket.cpp
)

target_include_directories(aardvark_core PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/aardvark)

target_include_directories(aardvark_core INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include)
    
target_link_libraries(aardvark_core
    skia ICU ICU_DATA fmt spdlog::spdlog nlohmann_json nod boost)

if(ADV_PLATFORM STREQUAL "linux")
    target_compile_definitions(aardvark_core PRIVATE "ADV_PLATFORM_DESKTOP=1")
    target_sources(aardvark_core PRIVATE
        src/platforms/desktop/desktop_window.cpp
        src/platforms/desktop/desktop_app.cpp
        src/utils/files_utils.cpp)
    target_link_libraries(aardvark_core glfw ${X11_LIBRARIES} stdc++fs)
endif()

if(ADV_PLATFORM STREQUAL "android")
    target_compile_definitions(aardvark_core PRIVATE "ADV_PLATFORM_ANDROID=1")
    target_sources(aardvark_core PRIVATE
        src/platforms/android/android_app.cpp
        src/platforms/android/android_jni.cpp
        src/platforms/android/android_binary_channel.cpp)
        # src/platforms/android/android_texture_layer.cpp)
    target_link_libraries(aardvark_core android EGL GLESv2 log)
endif()

if(ADV_CORE_EXAMPLES)
    # add_executable(align_example src/examples/align_example.cpp)
    # target_link_libraries(align_example aardvark)

    # add_executable(app_example examples/app_example.cpp)
    # target_link_libraries(app_example aardvark_core)

    # add_executable(border_example src/examples/border_example.cpp)
    # target_link_libraries(border_example aardvark)

    # add_executable(clip_example src/examples/clip_example.cpp)
    # target_link_libraries(clip_example aardvark)

    add_executable(flex_example examples/flex_example.cpp)
    target_link_libraries(flex_example aardvark_core)

    # add_executable(layer_example src/examples/layer_example.cpp)
    # target_link_libraries(layer_example aardvark)

    # add_executable(paint_cache_example src/examples/paint_cache.cpp)
    # target_link_libraries(paint_cache_example aardvark)

    # add_executable(responder_example src/examples/responder_example.cpp)
    # target_link_libraries(responder_example aardvark)

    # add_executable(paragraph_example src/examples/paragraph_example.cpp)
    # target_link_libraries(paragraph_example aardvark)

    # add_executable(websocket_example src/examples/websocket_example.cpp)
    # target_link_libraries(websocket_example aardvark)
endif()

if(ADV_CORE_TESTS)
    add_executable(tests
        src/tests/index.cpp
        src/tests/base_types_test.cpp
        src/tests/box_constraints_test.cpp
        # src/tests/responder_test.cpp
        # src/tests/align_test.cpp
        src/tests/text_span_test.cpp
        src/tests/element_observer_test.cpp
        src/tests/event_loop_test.cpp
    )
    target_link_libraries(aardvark_core_tests Catch2 aardvark_core)
endif()
