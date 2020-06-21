#include <string>
#include <nlohmann_json.hpp>
#include <android/log.h>
#include <jni.h>

#include "aardvark_js/android_host.hpp"
#include "aardvark/platforms/android/android_app.hpp"
#include "aardvark/base_types.hpp"
#include "aardvark/elements/elements.hpp"

using json = nlohmann::json;

extern "C" {
    JNIEXPORT void JNICALL Java_com_aardvark_example_AardvarkExampleActivity_init(
        JNIEnv* env, jobject obj, jlong app_ptr);
};

void handler(json message, void* user_data) {
    auto action = message["action"].get<int>();
    __android_log_print(ANDROID_LOG_VERBOSE, "AARDVARK", "Action is: %d", action);
    auto app = reinterpret_cast<aardvark::AndroidApp*>(user_data);
    auto aligned = dynamic_cast<aardvark::AlignedElement*>(app->document->root.get());
    auto sized = dynamic_cast<aardvark::SizedElement*>(aligned->child.get());
    auto background = dynamic_cast<aardvark::BackgroundElement*>(sized->child.get());
    if (action == 0) {
        background->color = aardvark::Color::from_sk_color(SK_ColorGREEN);
    } else if (action == 1) {
        background->color = aardvark::Color::from_sk_color(SK_ColorRED);
    }
    background->change();
}

JNIEXPORT void JNICALL Java_com_aardvark_example_AardvarkExampleActivity_init(
    JNIEnv* env, jobject obj, jlong host_ptr) {
    auto host = reinterpret_cast<aardvark::js::AndroidHost*>(host_ptr);
    auto app = host->app;
    
    // create elements
    auto alignment = aardvark::Alignment{
        aardvark::AlignmentOrigin::top_left,
        aardvark::Value::abs(500),
        aardvark::Value::abs(500)};
    auto size = aardvark::SizeConstraints{
        aardvark::Value::abs(300), aardvark::Value::abs(300)};
    auto background = std::make_shared<aardvark::BackgroundElement>(
        aardvark::Color::from_sk_color(SK_ColorRED));
    auto root = std::make_shared<aardvark::AlignedElement>(
        std::make_shared<aardvark::SizedElement>(background, size),
        alignment);
    app->document->set_root(root);

    // send message to the platform through the channel
    json msg;
    msg["str"] = "Hello";
    host->channel_manager.send_message("system", msg);

    // handle messages from the platform
    auto chan = host->channel_manager.get_channel<json>("system");
    chan->user_data = app.get();
    chan->set_message_handler(handler);
}
