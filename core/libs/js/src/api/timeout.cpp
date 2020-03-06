#include "api/timeout.hpp"

#include "host.hpp"

namespace aardvark::js {

void clear_timeout(
    Host& host, jsi::Value& this_val, std::vector<jsi::Value>& args) {
    if (args.size() == 0) return;
    auto id = args[0].to_number();
    if (!id.has_value()) return;
    host.event_loop->clear_timeout(static_cast<int>(id.value()));
}

jsi::Result<jsi::Value> set_timeout(
    Host& host, jsi::Value& this_val, std::vector<jsi::Value>& args) {
    auto err_params =
        jsi::CheckErrorParams{"argument", "callback", "setTimeout"};
    auto callback = host.api->EmptyCallback_mapper->try_from_js(
        *host.ctx, args[0], err_params);
    if (!callback.has_value()) {
        return jsi::make_error_result(*host.ctx, callback.error());
    }
    auto timeout = args.size() >= 2 ? args[1].to_number().value_or(0) : 0;
    auto id = host.event_loop->set_timeout(callback.value(), timeout);
    return host.ctx->value_make_number(id);
}

void add_timeout(jsi::Context& ctx) {
    auto host = static_cast<Host*>(ctx.user_pointer);
    auto set_timeout_fn =
        host->ctx
            ->object_make_function([host](auto this_val, auto args) {
                return set_timeout(*host, this_val, args);
            })
            .to_value();
    host->ctx->get_global_object().set_property("setTimeout", set_timeout_fn);
    auto clear_timeout_fn =
        host->ctx
            ->object_make_function([host](auto this_val, auto args) {
                clear_timeout(*host, this_val, args);
                return host->ctx->value_make_undefined();
            })
            .to_value();
    host->ctx->get_global_object().set_property("clearTimeout", clear_timeout_fn);
}

}  // namespace aardvark::js
