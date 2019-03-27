#pragma once

#include <functional>

namespace aardvark {

// Сhannel for exchanging messages between platform and native side
class BinaryChannel {
  public:
    // Sends message through the channel to the platform side
    virtual void send_message(void* message, int length){};

    virtual void set_message_handler(std::function<void(void*, int)> handler){};

    // Handles message received from the platform side
    virtual void handle_message(void* message, int length){};
};

template <class T>
class MessageCodec {
  public:
    virtual void* encode(T message, int* length){};
    virtual T decode(void* data, int length){};
};

template <class T>
class MessageChannel {
  public:
    MessageChannel(BinaryChannel binary_channel, MessageCodec<T> codec)
        : binary_channel(binary_channel), codec(codec) {
        binary_channel.set_message_handler(this->handle_message);
    };

    virtual void send_message(T message) {
        int length;
        auto buffer = codec.encode(message, &length);
        binary_channel.send_message(buffer, length);
    };

    virtual void handle_message(void* buffer, int length) {
        if (handler != nullptr) handler(codec.decode(buffer, length));
    };

    virtual void set_message_handler(std::function<void(T)> handler) {
        this->handler = handler;
    };

  private:
    BinaryChannel binary_channel;
    MessageCodec<T> codec;
    std::function<void(T)> handler;
};

}  // namespace aardvark
