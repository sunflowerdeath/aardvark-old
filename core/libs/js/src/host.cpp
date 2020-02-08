#include "host.hpp"

#include <aardvark/utils/log.hpp>

namespace aardvark::js {

Host::Host() {
    ctx = Qjs_Context::make();
    api = aardvark_js_api::Api(ctx.get());
    event_loop = std::make_shared<EventLoop>();
    module_loader =
        ModuleLoader(event_loop, ctx.get(), true, [](jsi::Error& err) {
            handle_error(err);
        });

    app = std::make_shared<DesktopApp>(event_loop);
    ctx->get_global_object().set_property("application",
        api->DesktopApp_mapper->to_js(*ctx, app));
}

Host::~Host() {
    stop();
    ~ctx.reset();
}

void Host::run() {
    if (is_running) return;
    is_running = true;
    event_loop->run();
    app->run([&]() {
        // animation_frame->call_callbacks();
    });
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
