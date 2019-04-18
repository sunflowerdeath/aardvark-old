#pragma once

#include <vector>
#include <functional>
#include <nlohmann_json.hpp>

namespace aardvark {

using json = nlohmann::json;

// Сhannel for exchanging binary messages between platform and native side
class BinaryChannel {
  public:
    // Sends message through the channel to the platform side
    virtual void send_message(const std::vector<char>& message){};

    virtual void set_message_handler(
        std::function<void(const std::vector<char>&)> handler) {
        this->handler = handler;
    }

    // Handles message received from the platform side
    void handle_message(const std::vector<char>& message) {
        if (handler != nullptr) handler(message);
    };

  private:
    std::function<void(const std::vector<char>&)> handler;
};

template <class T>
class MessageCodec {
  public:
    virtual std::vector<char> encode(const T& message){};
    virtual T decode(const std::vector<char>& message){};
};

class StringCodec : public MessageCodec<std::string> {
  public:
    std::vector<char> encode(const std::string& message) override {
        return std::vector<char>(message.begin(), message.end());
    };

    std::string decode(const std::vector<char>& message) override {
        return std::string(message.begin(), message.end());
    };

    static StringCodec* get_instance() {
        static StringCodec instance;
        return &instance;
    };
};

class JsonCodec : public MessageCodec<json> {
  public:
    std::vector<char> encode(const json& message) {
        return StringCodec::get_instance()->encode(message.dump());
    };

    json decode(const std::vector<char>& message) {
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
        binary_channel->set_message_handler([=](std::vector<char> message) {
            this->handle_message(message);
        });
    };

    void send_message(T message) {
        binary_channel->send_message(codec->encode(message));
    };

    void handle_message(const std::vector<char>& message) {
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
