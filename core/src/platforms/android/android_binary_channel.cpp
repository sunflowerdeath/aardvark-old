#include "android_binary_channel.hpp"

namespace aardvark {

JNIEnv* jni_env;
jclass channel_class;
jmethodID channel_handle_message_method;
jfieldID channel_native_addr_field;

void AndroidBinaryChannel::init_jni(JNIEnv* env) {
    jni_env = env;
    channel_class = env->FindClass("com/aardvark/BinaryChannel");
    channel_handle_message_method = env->GetMethodID(
        channel_class, "handleMessage", "(Ljava/nio/ByteBuffer;)V");
    channel_native_addr_field =
        env->GetFieldID(channel_class, "nativeChannelAddr", "J" /* long */);
}

AndroidBinaryChannel::AndroidBinaryChannel(jobject platform_channel) {
    this->platform_channel = jni_env->NewGlobalRef(platform_channel);
}

AndroidBinaryChannel* AndroidBinaryChannel::get_native_channel(
    jobject platform_channel) {
    auto addr =
        jni_env->GetLongField(platform_channel, channel_native_addr_field);
    return reinterpret_cast<AndroidBinaryChannel*>(addr);
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
