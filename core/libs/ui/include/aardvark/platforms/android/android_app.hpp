#pragma once

#include <jni.h>

#include <map>
#include <memory>

#include "aardvark/document.hpp"

namespace aardvark {

class AndroidApp {
  public:
    AndroidApp(jobject activity, int width, int height);

    void update();

    int width;
    int height;
    jobject activity;
    std::shared_ptr<Document> document;
};

}  // namespace aardvark
