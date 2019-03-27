#include "android_app.hpp"
#include "android_binary_channel.hpp"

namespace aardvark {

AndroidApp::AndroidApp(jobject activity, int width, int height)
    : activity(activity), width(width), height(height) {
    //AndroidBinaryChannel::init_jni(env);
    document = std::make_shared<Document>();
}

void AndroidApp::update() {
    document->paint();
}

}  // namespace aardvark
