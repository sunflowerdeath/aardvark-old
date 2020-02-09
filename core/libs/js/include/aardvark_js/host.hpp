#pragma once

#include <aardvark/platforms/desktop/desktop_app.hpp>
#include <aardvark/utils/event_loop.hpp>
#include <aardvark_jsi/jsi.hpp>
#include <aardvark_jsi/qjs.hpp>

#include "../generated/api.hpp"
#include "module_loader.hpp"

namespace aardvark::js {

class Host {
  public:
    Host();
    ~Host();

    void run();
    void stop();
    void handle_error(jsi::Error& err);

    std::shared_ptr<jsi::Context> ctx;
    std::optional<aardvark_js_api::Api> api;
    std::shared_ptr<EventLoop> event_loop;
    std::optional<ModuleLoader> module_loader;
    std::shared_ptr<DesktopApp> app;
    bool is_running = false;
};

}  // namespace aardvark::js
