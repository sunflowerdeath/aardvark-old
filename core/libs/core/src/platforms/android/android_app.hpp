# pragma once

#include <memory>
#include <map>
#include "jni.h"
#include "../../document.hpp"
#include "../../channels.hpp"

namespace aardvark {

class AndroidApp {
  public:
    AndroidApp(jobject activity, jobject system_channel, int width, int height);
    void update();
    int width;
    int height;
    std::shared_ptr<Document> document;
    jobject activity;

    template<class T>
    void register_channel(std::string name, MessageChannel<T>* channel) {
        channels[name] = channel;
    };

    template<class T>
    MessageChannel<T>* get_channel(const std::string& name) {
        return reinterpret_cast<MessageChannel<T>*>(channels[name]);
    };

    template<class T>
    void send_message(const std::string& channel, T message) {
        get_channel<T>(channel)->send_message(message);
    };

  private:
    std::map<std::string, void*> channels;
};

}  // namespace aardvark
