#include "android_app.hpp"

namespace aardvark {

#define LOG_INFO(...) ((void)__android_log_print(ANDROID_LOG_INFO, "aardvark", __VA_ARGS__))
#define LOG_WARN(...) ((void)__android_log_print(ANDROID_LOG_WARN, "aardvark", __VA_ARGS__))

// https://stackoverflow.com/a/39442473
void handle_app_cmd(android_app* native_app, int32_t cmd) {
    AndroidApp* app = (AndroidApp*)native_app->userData;
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            LOG_INFO("APP_CMD_INIT_WINDOW");
            app->init_window();
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            LOG_INFO("APP_CMD_TERM_WINDOW");
            app->term_window();
            break;
        case APP_CMD_WINDOW_RESIZED:
            LOG_INFO("APP_CMD_WINDOW_RESIZED");
            break;
        case APP_CMD_CONTENT_RECT_CHANGED:
            LOG_INFO("APP_CMD_CONTENT_RECT_CHANGED");
            break;
        case APP_CMD_GAINED_FOCUS:
            LOG_INFO("APP_CMD_GAINED_FOCUS");
            // app->is_active = true;
            break;
        case APP_CMD_LOST_FOCUS:
            LOG_INFO("APP_CMD_LOST_FOCUS");
            // app->is_active = false;
            break;
    }
}

AndroidApp::AndroidApp(android_app* native_app)
    : native_app(native_app) {
    native_app->userData = (void*)this;
    native_app->onAppCmd = handle_app_cmd;
}

void AndroidApp::run() {
    should_stop = false;
    while (!should_stop) {
        android_poll_source* source;
        // Value is >= 0 until all events are handled
        while (ALooper_pollAll(is_active ? 0 : -1,  // timeout
                               nullptr,             // outFd
                               nullptr,             // outEvents
                               (void**)&source      // outData
                               ) >= 0) {
            // Handle event
            if (source != nullptr) {
            	// This will call native_app->onAppCmd
            	source->process(native_app, source);
            }
        }
        if (is_active) {
            auto painted = document->paint();
            if (painted) window->swap();
        }
    }
}

void AndroidApp::stop() {
    should_stop = true;
}

void AndroidApp::init_window() {
    is_active = true;
    window = std::make_shared<AndroidWindow>(native_app->window);
    // TODO refactor
    if (root->layer_tree != nullptr) root->layer_tree->children.clear();
    document = std::make_shared<Document>(root);
}

void AndroidApp::term_window() {
    is_active = false;
    window.reset();
    document.reset();
}

}  // namespace aardvark
