#pragma once

#include <jni.h>

#include <aardvark/channels.hpp>
#include <functional>

namespace aardvark {

// Сhannel for exchanging messages between Android and native side
class AndroidBinaryChannel : public BinaryChannel {
  public:
    AndroidBinaryChannel(jobject platform_channel);

    void send_message(const std::vector<char>& message) override;

    // This method should be called once before using this class to initalize
    // JNI bindings.
    static void init_jni(JNIEnv* env);

    static AndroidBinaryChannel* get_native_channel(jobject platform_channel);

  private:
    jobject platform_channel;
};

}  // namespace aardvark
