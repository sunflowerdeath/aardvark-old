#pragma once

#include <aardvark_jsi/jsi.hpp>
#include <functional>
#include <map>

namespace aardvark::js {

class AnimationFrame {
  public:
    int add_callback(std::function<void()> callback) {
        id++;
        callbacks[id] = std::move(callback);
        return id;
    };

    void remove_callback(int id) {
        callbacks.erase(id);
    };

    void call_callbacks() {
        // Copy because list of callbacks can be modified during the call
        auto copy = callbacks;
        callbacks.clear();
        for (auto& it : copy) it.second();
    };

  private:
    int id = 0;
    std::map<int, std::function<void()>> callbacks;
};

int request_animation_frame(jsi::Context& ctx, std::function<void()> callback);

void cancel_animation_frame(jsi::Context& ctx, int id);

}  // namespace aardvark::js
