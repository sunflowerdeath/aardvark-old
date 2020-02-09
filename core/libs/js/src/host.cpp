#include "host.hpp"

#include <aardvark/utils/log.hpp>

namespace aardvark::js {

Host::Host() {
    ctx = jsi::Qjs_Context::create();
    api.emplace(ctx.get());
    event_loop = std::make_shared<EventLoop>();
    module_loader = ModuleLoader(
        event_loop.get(), ctx.get(), true, [this](jsi::Error& err) {
            handle_error(err);
        });

    app = std::make_shared<DesktopApp>(event_loop);
    ctx->get_global_object().set_property("application",
        api->DesktopApp_mapper->to_js(*ctx, app));
}

Host::~Host() {
    stop();
    ctx.reset();
}

void Host::run() {
    if (is_running) return;
    is_running = true;
    app->run([&]() {
        // animation_frame->call_callbacks();
    });
    event_loop->run();
}

void Host::stop() {
    if (!is_running) return;
    is_running = false;
    app->stop();
    event_loop->stop();
}

void Host::handle_error(jsi::Error& err) {
    Log::error(err.message());
    stop();
}

}  // namespace aardvark::js
