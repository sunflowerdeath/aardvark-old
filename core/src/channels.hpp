#pragma once

#include <cstring>
#include <functional>
#include <nlohmann_json.hpp>

namespace aardvark {

using json = nlohmann::json;

// Very simple class that holds binary data
class BinaryBuffer {
  public:
    BinaryBuffer(void* src_data, int length) : length(length) {
        data = malloc(length);
        std::memcpy(data, src_data, length);
    }

    ~BinaryBuffer() {
        free(data);
    }

    int length;
    void* data;
};

// Сhannel for exchanging messages between platform and native side
class BinaryChannel {
  public:
    // Sends message through the channel to the platform side
    virtual void send_message(const BinaryBuffer& message){};

    virtual void set_message_handler(
        std::function<void(const BinaryBuffer&)> handler) {
        this->handler = handler;
    }

    // Handles message received from the platform side
    void handle_message(const BinaryBuffer& message) {
        if (handler != nullptr) handler(message);
    };

  private:
    std::function<void(const BinaryBuffer&)> handler;
};

template <class T>
class MessageCodec {
  public:
    virtual BinaryBuffer encode(const T& message){};
    virtual T decode(const BinaryBuffer& message){};
};

class StringCodec : public MessageCodec<std::string> {
  public:
    BinaryBuffer encode(const std::string& message) override {
        return BinaryBuffer(const_cast<char*>(message.data()), message.size());
    };

    std::string decode(const BinaryBuffer& message) override {
        return std::string(static_cast<const char*>(message.data),
                           message.length);
    };

    static StringCodec* get_instance() {
        static StringCodec instance;
        return &instance;
    };
};

class JsonCodec : public MessageCodec<json> {
  public:
    BinaryBuffer encode(const json& message) {
        return StringCodec::get_instance()->encode(message.dump());
    };

    json decode(const BinaryBuffer& message) {
        auto str = StringCodec::get_instance()->decode(message);
        return json::parse(str);
    };

    static JsonCodec* get_instance() {
        static JsonCodec instance;
        return &instance;
    };
};

template <class T>
class MessageChannel {
  public:
    MessageChannel(BinaryChannel* binary_channel, MessageCodec<T>* codec)
        : binary_channel(binary_channel), codec(codec) {
        binary_channel->set_message_handler([=](BinaryBuffer message) {
            this->handle_message(message);
        });
    };

    void send_message(T message) {
        binary_channel->send_message(codec->encode(message));
    };

    void handle_message(const BinaryBuffer& message) {
        if (handler != nullptr) {
            handler(codec->decode(message), user_data);
        }
    };

    void set_message_handler(std::function<void(T, void*)> handler) {
        this->handler = handler;
    };

    void* user_data;

  private:
    BinaryChannel* binary_channel;
    MessageCodec<T>* codec;
    std::function<void(T, void*)> handler;
};

}  // namespace aardvark
