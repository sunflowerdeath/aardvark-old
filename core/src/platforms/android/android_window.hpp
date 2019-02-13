#pragma once

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>

namespace aardvark {

class AndroidWindow {
  public:
    AndroidWindow(ANativeWindow* native_window);
    ~AndroidWindow();
    // Disable copy and assignment
    AndroidWindow(const AndroidWindow&) = delete;
    AndroidWindow& operator=(AndroidWindow const&) = delete;

    ANativeWindow* native_window;
    EGLDisplay display;
    EGLContext context;
    EGLSurface surface;
    EGLint width;
    EGLint height;

    void make_current();
    void swap();
};

}  // namespace aardvark

