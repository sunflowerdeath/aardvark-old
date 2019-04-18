#include <string>
#include <nlohmann_json.hpp>
#include <android/log.h>
#include <jni.h>

#include "platforms/android/android_app.hpp"
#include "base_types.hpp"
#include "elements/elements.hpp"

using json = nlohmann::json;

extern "C" {
    JNIEXPORT void JNICALL Java_com_aardvark_example_AardvarkExampleActivity_init(
        JNIEnv* env, jobject obj, jlong app_ptr);
};

void handler(json message, void* user_data) {
    auto action = message["action"].get<int>();
    __android_log_print(ANDROID_LOG_VERBOSE, "AARDVARK", "Action is: %d", action);
    auto app = reinterpret_cast<aardvark::AndroidApp*>(user_data);
    auto align = dynamic_cast<aardvark::elements::Align*>(app->document->root.get());
    auto size = dynamic_cast<aardvark::elements::FixedSize*>(align->child.get());
    auto background = dynamic_cast<aardvark::elements::Background*>(size->child.get());
    if (action == 0) {
        background->color = SK_ColorGREEN;
    } else if (action == 1) {
        background->color = SK_ColorRED;
    }
    background->change();
}

JNIEXPORT void JNICALL Java_com_aardvark_example_AardvarkExampleActivity_init(
    JNIEnv* env, jobject obj, jlong app_ptr) {
    auto app = reinterpret_cast<aardvark::AndroidApp*>(app_ptr);
    
    // create elements
    auto insets = aardvark::elements::EdgeInsets{aardvark::Value::abs(500), aardvark::Value::abs(500)};
    auto background = std::make_shared<aardvark::elements::Background>(SK_ColorRED);
    auto root = std::make_shared<aardvark::elements::Align>(
        std::make_shared<aardvark::elements::FixedSize>(background, aardvark::Size{300, 300}),
        insets);
    app->document->set_root(root);

    // send message to the platform through the channel
    json msg;
    msg["str"] = "Hello";
    app->send_message("system", msg);

    // handle messages from the platform
    auto chan = app->get_channel<json>("system");
    chan->user_data = app;
    chan->set_message_handler(handler);
}