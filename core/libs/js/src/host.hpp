#pragma once

#include <aardvark_jsi/jsi.hpp>
#include "../platforms/desktop/desktop_app.hpp"
#include "../utils/event_loop.hpp"
#include "module_loader.hpp"
#include "api.hpp"

namespace aardvark::js {

class Host {
  public:
    Host();
    ~Host();

    void run();
    void stop();

    Context ctx;
    Api api;
    std::unique_ptr<ModuleLoader> module_loader;
    std::shared_ptr<EventLoop> event_loop;
};

}  // namespace aardvark::js
