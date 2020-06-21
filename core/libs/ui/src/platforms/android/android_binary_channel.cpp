#include "platforms/android/android_binary_channel.hpp"

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

void AndroidBinaryChannel::send_message(const std::vector<char>& message) {
    auto buffer = jni_env->NewDirectByteBuffer(
        const_cast<char*>(message.data()), message.size());
    jni_env->CallVoidMethod(
        platform_channel, channel_handle_message_method, buffer);
}

}  // namespace aardvark
