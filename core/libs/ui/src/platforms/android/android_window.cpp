#include "platforms/android/android_window.hpp"

#include <EGL/egl.h>

namespace aardvark {

AndroidWindow::AndroidWindow(ANativeWindow* native_window) {
    surface = eglCreateWindowSurface(display, config, native_window, NULL);
    eglMakeCurrent(display, surface, surface, context);
}

AndroidWindow::~AndroidWindow() {
    if (surface != EGL_NO_SURFACE) eglDestroySurface(display, surface);
}

}
