#include <string>
#include <nlohmann_json.hpp>
#include "jni.h"
#include "base_types.hpp"
#include "elements/elements.hpp"
#include "platforms/android/android_app.hpp"
#include "platforms/android/android_binary_channel.hpp"

using json = nlohmann::json;

extern "C" {
    JNIEXPORT void JNICALL Java_com_aardvark_AardvarkActivity_init(
        JNIEnv* env, jobject obj, jlong app_ptr, jobject platform_channel);
};

struct AppState {
    std::shared_ptr<aardvark::elements::Background> background;
};

void handler(json message, void* user_data) {
    auto state = reinterpret_cast<AppState*>(user_data);
    state->background->color = state->background->color == SK_ColorRED ? SK_ColorGREEN : SK_ColorRED;
    state->background->change();
}

JNIEXPORT void JNICALL Java_com_aardvark_AardvarkActivity_init(
    JNIEnv* env, jobject obj, jlong app_ptr, jobject platform_channel) {
    auto app = reinterpret_cast<aardvark::AndroidApp*>(app_ptr);
    auto insets = aardvark::elements::EdgeInsets{aardvark::Value::abs(500), aardvark::Value::abs(500)};
    auto background = std::make_shared<aardvark::elements::Background>(SK_ColorRED);
    auto root = std::make_shared<aardvark::elements::Align>(
        std::make_shared<aardvark::elements::FixedSize>(background, aardvark::Size{300, 300}),
        insets);
    app->document->set_root(root);

    auto app_state = AppState{background};

    auto binary_channel = aardvark::AndroidBinaryChannel::get_native_channel(platform_channel);
    auto channel = new aardvark::MessageChannel<json>(binary_channel, aardvark::JsonCodec::get_instance());
    app->register_channel("system", channel);

    // send message to the platform through the channel
    json msg;
    msg["str"] = "Hello";
    app->send_message("system", msg);

    // handle messages from the platform
    auto chan = app->get_channel<json>("system");
    chan->user_data = static_cast<void*>(&app_state);
    chan->set_message_handler(handler);
}