#include "android_window.hpp"

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

AndroidWindow::AndroidWindow(ANativeWindow* native_window)
    : native_window(native_window) {
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, 0, 0);
    auto config = select_egl_config(display);
    const EGLint EGLContextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
    context = eglCreateContext(display, config, NULL, EGLContextAttribs);
    surface = eglCreateWindowSurface(display, config, native_window, NULL);
    make_current();
}

AndroidWindow::~AndroidWindow() {
    if (display == EGL_NO_DISPLAY) return;
    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (context != EGL_NO_CONTEXT) eglDestroyContext(display, context);
    if (surface != EGL_NO_SURFACE) eglDestroySurface(display, surface);
    eglTerminate(display);
}

void AndroidWindow::make_current() {
    eglMakeCurrent(display, surface, surface, context);
}

void AndroidWindow::swap() {
   	eglSwapBuffers(display, surface);
}

}  // namespace aardvark
