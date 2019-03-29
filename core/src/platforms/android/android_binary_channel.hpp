#pragma once

#include <functional>
#include "jni.h"
#include "../../channels.hpp"

namespace aardvark {

// Сhannel for exchanging messages between Android and native side
class AndroidBinaryChannel : public BinaryChannel {
  public:
    AndroidBinaryChannel(jobject platform_channel);

    void send_message(void* message, int length) override;
    void set_message_handler(std::function<void(void*, int)> handler) override;
    void handle_message(void* message, int length) override;

    // This method should be called once before using this class to initalize
    // JNI bindings.
    static void init_jni(JNIEnv* env);

    static AndroidBinaryChannel* get_native_channel(jobject platform_channel);

  private:
    jobject platform_channel;
    std::function<void(void*, int)> handler;
};

}  // namespace aardvark
