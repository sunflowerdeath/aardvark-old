#include "platforms/android/android_app.hpp"

// #include "platforms/android/android_binary_channel.hpp"
#include <android/native_window_jni.h>
#include <android/log.h>

#define LOG_TAG "aardvark"

#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace aardvark {

EGLConfig select_egl_config(EGLDisplay display) {
	// Has at least 8 bits per color and supports creating window surfaces
    // clang-format off
    const EGLint attribs[] = {
    	EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE // The list is terminated with EGL_NONE
    };
    // clang-format on
    EGLConfig config;
    EGLint num_configs = 0;
    eglChooseConfig(display, attribs, &config, 1, &num_configs);
    // num_configs must be > 0
    return config;
}

AndroidApp::AndroidApp(JNIEnv* env, jobject activity, jobject jsurface) {
    eglBindAPI(EGL_OPENGL_ES_API);

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        LOG_ERROR("eglGetDisplay() returned error %d", eglGetError());
    }

    auto init_res = eglInitialize(display, 0, 0);
    if (init_res) {
        LOG_ERROR("eglInitialize() returned error %d", eglGetError());
    }

    auto config = select_egl_config(display);

    const EGLint EGLContextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
    context = eglCreateContext(display, config, nullptr, EGLContextAttribs);
    if (!context) LOG_ERROR("eglCreateContext() returned error %d", eglGetError());
    
    auto native_window = ANativeWindow_fromSurface(env, jsurface);
    // window = std::make_shared<AndroidWindow>(native_window);

    surface = eglCreateWindowSurface(display, config, native_window, NULL);
    if (!surface) LOG_ERROR("eglCreateWindowSurface() returned error %d", eglGetError());

    auto res = eglMakeCurrent(display, surface, surface, context);
    if (!res) LOG_ERROR("eglMakeCurrent() returned error %d", eglGetError());
    
    auto gr_context = GrContext::MakeGL();
    auto screen = Layer::make_screen_layer(gr_context);
    document = std::make_shared<Document>(gr_context, screen);
}

AndroidApp::~AndroidApp() {
    if (surface != EGL_NO_SURFACE) eglDestroySurface(display, surface);
    // ANativeWindow_release
    // TODO
}

void AndroidApp::update() {
    auto rendered = document->render();
    if (rendered) {
        if (!eglSwapBuffers(display, surface)) {
            LOG_ERROR("eglSwapBuffers() returned error %d", eglGetError());
        }
    }

}

// void AndroidApp::handle_system_message(json message) { document->handle_event
// }

}  // namespace aardvark
