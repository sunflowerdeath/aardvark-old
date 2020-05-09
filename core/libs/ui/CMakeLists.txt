cmake_minimum_required(VERSION 3.10)

project(ADV_UI LANGUAGES C CXX)

#-------------------------------------------------------------------------------
# TARGETS
#-------------------------------------------------------------------------------
# Library
if (ADV_UI_SHARED)
    set(ADV_UI_LIB_TYPE SHARED)
else()
    set(ADV_UI_LIB_TYPE STATIC)
endif()

add_library(aardvark_ui ${ADV_UI_LIB_TYPE}
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
    src/elements/size.cpp
    src/elements/stack.cpp
    src/elements/text.cpp
    src/elements/translate.cpp
    src/pointer_events/hit_tester.cpp
    src/pointer_events/pointer_event_manager.cpp
    src/utils/event_loop.cpp
    src/utils/websocket.cpp
)

target_include_directories(aardvark_ui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/aardvark)

target_include_directories(aardvark_ui INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include)
    
target_link_libraries(aardvark_ui
    skia ICU ICU_DATA fmt spdlog::spdlog nlohmann_json nod boost)

if(ADV_PLATFORM STREQUAL "linux")
    target_compile_definitions(aardvark_ui PRIVATE "ADV_PLATFORM_DESKTOP=1")
    target_sources(aardvark_ui PRIVATE
        src/platforms/desktop/desktop_window.cpp
        src/platforms/desktop/desktop_app.cpp
        src/utils/files_utils.cpp)
    target_link_libraries(aardvark_ui glfw ${X11_LIBRARIES} stdc++fs)
endif()

if(ADV_PLATFORM STREQUAL "android")
    target_compile_definitions(aardvark_ui PRIVATE "ADV_PLATFORM_ANDROID=1")
    target_sources(aardvark_ui PRIVATE
        src/platforms/android/android_app.cpp
        src/platforms/android/android_jni.cpp
        src/platforms/android/android_binary_channel.cpp)
        # src/platforms/android/android_texture_layer.cpp)
    target_link_libraries(aardvark_ui android EGL GLESv2 log)
endif()

if(ADV_UI_EXAMPLES)
    # add_executable(align_example src/examples/align_example.cpp)
    # target_link_libraries(align_example aardvark)

    # add_executable(app_example examples/app_example.cpp)
    # target_link_libraries(app_example aardvark_ui)

    # add_executable(border_example src/examples/border_example.cpp)
    # target_link_libraries(border_example aardvark)

    # add_executable(clip_example src/examples/clip_example.cpp)
    # target_link_libraries(clip_example aardvark)

    add_executable(adv_ui_flex_example examples/flex_example.cpp)
    target_link_libraries(adv_ui_flex_example aardvark_ui)

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

if(ADV_UI_TESTS)
    add_executable(adv_ui_tests
        tests/index.cpp
        tests/base_types_test.cpp
        tests/box_constraints_test.cpp
        # tests/responder_test.cpp
        # tests/align_test.cpp
        tests/text_span_test.cpp
        tests/element_observer_test.cpp
        tests/event_loop_test.cpp
    )
    target_link_libraries(adv_ui_tests Catch2 aardvark_ui)
endif()
