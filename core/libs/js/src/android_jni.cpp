#include <jni.h>

#include "aardvark_js/android_host.hpp"
#include "aardvark/platforms/android/android_binary_channel.hpp"

extern "C" {

JNIEXPORT void JNICALL
Java_com_aardvark_NativeWrapper_initJni(JNIEnv* env, jobject obj);

JNIEXPORT jlong JNICALL Java_com_aardvark_NativeWrapper_hostCreate(
    JNIEnv* env,
    jobject wrapper,
    jobject activity,
    jobject system_channel,
    jobject surface);

JNIEXPORT void JNICALL Java_com_aardvark_NativeWrapper_hostUpdate(
    JNIEnv* env, jobject obj, jlong host_ptr);

JNIEXPORT jlong JNICALL Java_com_aardvark_NativeWrapper_channelCreate(
    JNIEnv* env, jobject obj, jobject platform_channel);

JNIEXPORT void JNICALL Java_com_aardvark_NativeWrapper_channelHandleMessage(
    JNIEnv* env, jobject obj, jlong channel_ptr, jobject data);

};

JNIEXPORT void JNICALL
Java_com_aardvark_NativeWrapper_initJni(JNIEnv* env, jobject obj) {
    aardvark::AndroidBinaryChannel::init_jni(env);
};

JNIEXPORT jlong JNICALL Java_com_aardvark_NativeWrapper_hostCreate(
    JNIEnv* env,
    jobject wrapper,
    jobject activity,
    jobject system_channel,
    jobject surface) {
    auto host =
        new aardvark::js::AndroidHost(env, system_channel, activity, surface);
    return reinterpret_cast<jlong>(host);
}

JNIEXPORT void JNICALL Java_com_aardvark_NativeWrapper_hostUpdate(
    JNIEnv* env, jobject caller, jlong host_ptr) {
    auto host = reinterpret_cast<aardvark::js::AndroidHost*>(host_ptr);
    host->update();
}

JNIEXPORT jlong JNICALL Java_com_aardvark_NativeWrapper_channelCreate(
    JNIEnv* env, jobject obj, jobject platform_channel) {
    auto channel = new aardvark::AndroidBinaryChannel(platform_channel);
    return reinterpret_cast<jlong>(channel);
}

JNIEXPORT void JNICALL Java_com_aardvark_NativeWrapper_channelHandleMessage(
    JNIEnv* env, jobject obj, jlong chan_ptr, jobject buffer) {
    auto channel = reinterpret_cast<aardvark::AndroidBinaryChannel*>(chan_ptr);
    auto data = reinterpret_cast<char*>(env->GetDirectBufferAddress(buffer));
    auto length = env->GetDirectBufferCapacity(buffer);
    auto message = std::vector<char>(data, data + length);
    channel->handle_message(message);
}
