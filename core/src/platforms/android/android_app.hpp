#pragma once

#include <memory>
#include <android/log.h>
#include <android_native_app_glue.h>
#include "../../document.hpp"
#include "../../element.hpp"
#include "android_window.hpp"

namespace aardvark {

class AndroidApp {
  public:
    AndroidApp(struct android_app* native_app);
    ~AndroidApp(){};

    std::shared_ptr<Element> root;
    std::shared_ptr<AndroidWindow> createWindow();
    std::shared_ptr<Document> getWindowDocument(AndroidWindow*);
    void run();
    void stop();

    void init_window();
    void term_window();
    bool is_active = false;

  private:
    android_app* native_app;
    bool should_stop = false;
    std::shared_ptr<AndroidWindow> window;
    std::shared_ptr<Document> document;
};

}  // namespace aardvark

