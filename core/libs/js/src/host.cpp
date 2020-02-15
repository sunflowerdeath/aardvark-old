#include "host.hpp"

#include <aardvark_jsi/check.hpp>
#include <aardvark/utils/log.hpp>
#include <iostream>

namespace aardvark::js {

void log(jsi::Context& ctx, std::vector<jsi::Value>& args) {
    for (auto& arg : args) {
        auto to_str = arg.to_string();
        if (to_str.has_value()) {
            std::cout << to_str.value().to_utf8();
        } else {
            std::cout << jsi::get_type_name(arg);
        }
        if (&arg != &args.back()) std::cout << " ";
    }
    std::cout << std::endl;
}

Host::Host() {
    ctx = jsi::Qjs_Context::create();
    api.emplace(ctx.get());
    event_loop = std::make_shared<EventLoop>();
    module_loader = ModuleLoader(
        event_loop.get(),
        ctx.get(),
        true,
        [this](
            jsi::Error& err,
            std::optional<jsi::ErrorLocation> original_location) {
            handle_error(err, original_location);
        });

    app = std::make_shared<DesktopApp>(event_loop);
    auto log_val =
        ctx->object_make_function(
               [& ctx = *ctx](
                   jsi::Value& this_val, std::vector<jsi::Value>& args) {
                   log(ctx, args);
                   return ctx.value_make_undefined();
               })
            .to_value();
    ctx->get_global_object().set_property("log", log_val);
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

void Host::handle_error(
    jsi::Error& err, std::optional<jsi::ErrorLocation> original_location) {
    Log::error("[JS] Uncaught exception:");
    Log::error(err.message());
    auto loc =
        original_location.has_value() ? original_location : err.location();
    if (loc.has_value()) {
        Log::error("Filename: {}", loc->source_url);
        Log::error("Line: {}, column: {}", loc->line, loc->column);
    }
    auto val = err.value();
    auto stack = val.to_object()
                     .value()
                     .get_property("stack")
                     .value()
                     .to_string()
                     .value()
                     .to_utf8();
    Log::error("Stacktrace:\n{}", stack);
    stop();
}

}  // namespace aardvark::js
