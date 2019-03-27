# pragma once

#include <memory>
#include "jni.h"
#include "../../document.hpp"

namespace aardvark {

class AndroidApp {
  public:
    AndroidApp(jobject activity, int width, int height);
    void update();
    int width;
    int height;
    std::shared_ptr<Document> document;
    jobject activity;
};

}  // namespace aardvark
