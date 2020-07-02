#include "android_host.hpp"

namespace aardvark::js {

/*
class JniEventLoop {
  public:
    JniEventLoop(MessageChannel<json>* channel) : channel(channel) {
        channel->set_message_handler(
            [](json& msg, void* user_data) { handle_message(msg); });
    }

    void handle(json& msg) {
        auto type = msg["type"].get<std::string>();
        if (type == "setTimeout") {
            auto id = msg["id"].get<int>();
            callbacks[id]();
            callbacks.erase(id);
        } else if (type == "clearTimeout") {
            auto id = msg["id"].get<int>();
            callbacks.erase(id);
        }
    }

    void set_timeout(std::function<void()> callback, int timeout) {
        auto id = current_id++;
        callbacks[id] = callback;
        json msg;
        msg["type"] = "setTimeout";
        msg["id"] = id;
        channel->send_message(msg);
    }

    void clear_timeout(int id) {
        callbacks.erase(id);
        json msg;
        msg["type"] = "clearTimeout";
        msg["id"] = id;
        channel->send_message(msg);
    }

  private:
    MessageChannel<json>* channel;
    int current_id = 0;
    std::unordered_map<int, std::function<void()>> callbacks;
}
*/

AndroidHost::AndroidHost(
    JNIEnv* env, jobject platform_channel, jobject activity, jobject surface) {
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

    /*
    auto timer_channel = channel_manager.get_channel<json>("timers");
    timer_channel->set_message_handler([](json& msg, void* user_data) {
        auto type = msg["type"].get<std::string>();
        if (type == "timeout") {
            auto cb = msg["id"].get<int>();
            timers[cb]();
        }
    });
    */

    app = std::make_shared<aardvark::AndroidApp>(env, activity, surface);
    auto global = ctx->get_global_object();
    global.set_property(
        "application", api->AndroidApp_mapper->to_js(*ctx, app));
    global.set_property("window", global.to_value());
}

void AndroidHost::update() {
    animation_frame.call_callbacks();
    app->update();
}

}  // namespace aardvark::js
