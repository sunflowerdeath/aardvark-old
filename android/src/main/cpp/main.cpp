#include <string>
#include "jni.h"
#include "base_types.hpp"
#include "elements/elements.hpp"
#include "platforms/android/android_app.hpp"
#include "platforms/android/android_binary_channel.hpp"

extern "C" {
    JNIEXPORT void JNICALL Java_com_aardvark_AardvarkActivity_init(JNIEnv* env, jobject obj, jlong app_ptr, jobject platform_channel);
};

JNIEXPORT void JNICALL Java_com_aardvark_AardvarkActivity_init(JNIEnv* env, jobject obj, jlong app_ptr, jobject platform_channel) {
    auto app = reinterpret_cast<aardvark::AndroidApp*>(app_ptr);
    auto insets = aardvark::elements::EdgeInsets{aardvark::Value::abs(500), aardvark::Value::abs(500)};
    auto root = std::make_shared<aardvark::elements::Align>(
        std::make_shared<aardvark::elements::FixedSize>(
            std::make_shared<aardvark::elements::Background>(SK_ColorRED), aardvark::Size{300, 300}),
        insets);
    app->document->set_root(root);

    auto channel = aardvark::AndroidBinaryChannel::get_native_channel(platform_channel);
    auto str = std::string("Hello");
    channel->send_message(str.data(), str.size());

}