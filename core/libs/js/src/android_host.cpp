#include "android_host.hpp"

namespace aardvark::js {

AndroidHost::AndroidHost(
    jobject platform_channel, jobject activity, int width, int height) {
    ctx = jsi::Qjs_Context::create();
    ctx->user_pointer = static_cast<void*>(this);

    // module_loader.emplace(
        // event_loop.get(),
        // ctx.get(),
        // true,
        // [this](
            // jsi::Error& err,
            // std::optional<jsi::ErrorLocation> original_location) {
            // // TODO
        // });

    api.emplace(ctx.get());
    api->error_handler = [this](jsi::Error& err) {
        // TODO
    };

    auto binary_channel =
        aardvark::AndroidBinaryChannel::get_native_channel(platform_channel);
    auto system_channel = new aardvark::MessageChannel<json>(
        binary_channel, aardvark::JsonCodec::get_instance());
    channel_manager.register_channel("system", system_channel);

    app = std::make_shared<aardvark::AndroidApp>(activity, width, height);
    auto global = ctx->get_global_object();
    global.set_property(
        "application", api->AndroidApp_mapper->to_js(*ctx, app));
    global.set_property("window", global.to_value());
};

}  // namespace aardvark::js
