#pragma once

#include <android/native_window_jni.h>

namespace aardvark {

class AndroidWindow {
  public:
    AndroidWindow(ANativeWindow* native_window);

  private:
    EGLSurface surface;
    EGLint width;
    EGLint height;
};

}  // namespace aardvark
