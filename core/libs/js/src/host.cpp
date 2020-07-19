#include "host.hpp"

#include <aardvark/utils/log.hpp>
#include <aardvark_jsi/check.hpp>
#include <iostream>

#include "api/timeout.hpp"

namespace aardvark::js {

void log(std::vector<jsi::Value>& args) {
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
    ctx->user_pointer = static_cast<void*>(this);

    api.emplace(ctx.get());
    api->error_handler = [this](jsi::Error& err) {
        // TODO original_location
        handle_error(err, /* original_location */ std::nullopt);
    };
    event_loop = std::make_shared<EventLoop>();
    module_loader = ModuleLoader(
        event_loop.get(),
        ctx.get(),
        true,
        [this](
            jsi::Error& err,
            std::optional<jsi::ErrorLocation> original_location) {
            handle_error(err, std::move(original_location));
        });

    auto global = ctx->get_global_object();
    app = std::make_shared<DesktopApp>(event_loop);
    global.set_property(
        "application", api->DesktopApp_mapper->to_js(*ctx, app));
    global.set_property("window", global.to_value());

    auto log_fn =
        ctx->object_make_function(
               [this](jsi::Value& this_val, std::vector<jsi::Value>& args) {
                   log(args);
                   return ctx->value_make_undefined();
               })
            .to_value();
    global.set_property("log", log_fn);

    auto gc_fn =
        ctx->object_make_function(
               [this](jsi::Value& this_val, std::vector<jsi::Value>& args) {
                   ctx->garbage_collect();
                   return ctx->value_make_undefined();
               })
            .to_value();
    global.set_property("gc", gc_fn);

    add_timeout(*ctx);
}

Host::~Host() {
    stop();
    event_loop.reset();
    module_loader = std::nullopt;
    ctx.reset();
    app.reset();
}

void Host::run() {
    if (is_running) return;
    is_running = true;
    app->run([&]() { animation_frame.call_callbacks(); });
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
