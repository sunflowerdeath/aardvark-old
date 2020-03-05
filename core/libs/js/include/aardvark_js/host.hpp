#pragma once

#include <aardvark/platforms/desktop/desktop_app.hpp>
#include <aardvark/utils/event_loop.hpp>
#include <aardvark_jsi/jsi.hpp>
#include <aardvark_jsi/qjs.hpp>

#include "../generated/api.hpp"
#include "module_loader.hpp"

namespace aardvark::js {

class AnimationFrame {
  public:
    int add_callback(std::function<void()> callback) {
        id++;
        callbacks[id] = std::move(callback);
        return id;
    };

    void remove_callback(int id) {
        callbacks.erase(id);
    };

    void call_callbacks() {
        // Copy because list of callbacks can be modified during the call
        auto copy = callbacks;
        callbacks.clear();
        for (auto& it : copy) it.second();
    };

  private:
    int id = 0;
    std::map<int, std::function<void()>> callbacks;
};

class Host {
  public:
    Host();
    ~Host();

    void run();
    void stop();
    void handle_error(jsi::Error& err, std::optional<jsi::ErrorLocation>);
    
    AnimationFrame animation_frame = AnimationFrame();
    std::shared_ptr<jsi::Context> ctx;
    std::optional<aardvark_js_api::Api> api;
    std::shared_ptr<EventLoop> event_loop;
    std::optional<ModuleLoader> module_loader;
    std::shared_ptr<DesktopApp> app;
    bool is_running = false;
};

}  // namespace aardvark::js
