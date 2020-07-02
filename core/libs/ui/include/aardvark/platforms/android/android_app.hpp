#pragma once

#include <jni.h>
#include <EGL/egl.h>
//#include <GLES/gl.h>
//#include <GLES2/gl2.h>
//#include <map>
#include <memory>
#include "aardvark/platforms/android/android_window.hpp"
#include "aardvark/document.hpp"

namespace aardvark {

class AndroidApp {
  public:
    AndroidApp(JNIEnv* env, jobject activity, jobject surface);
    ~AndroidApp();

    void update();

    jobject activity;
    EGLDisplay display;
    EGLContext context;
    EGLSurface surface;
    //std::shared_ptr<AndroidWindow> window;
    std::shared_ptr<Document> document;
};

}  // namespace aardvark
