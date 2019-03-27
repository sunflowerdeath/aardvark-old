#include "android_binary_channel.hpp"

namespace aardvark {

JNIEnv* jni_env;
jmethodID channel_handle_message_method;

void AndroidBinaryChannel::init_jni(JNIEnv* env) {
    jni_env = env;
    auto channel_class = env->FindClass("com/aardvark/BinaryChannel");
    channel_handle_message_method = env->GetMethodID(
        channel_class, "handleMessage", "(Ljava/nio/ByteBuffer;)V");
}

void AndroidBinaryChannel::send_message(void* message, int length) {
    auto buffer = jni_env->NewDirectByteBuffer(message, length);
    jni_env->CallVoidMethod(platform_channel, channel_handle_message_method,
                        buffer);
}

void AndroidBinaryChannel::set_message_handler(
    std::function<void(void*, int)> handler) {
    this->handler = handler;
}

void AndroidBinaryChannel::handle_message(void* message, int length) {
    if (handler != nullptr) handler(message, length);
}

}  // namespace aardvark
