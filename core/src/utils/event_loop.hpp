#pragma once

#include <functional>
#include <unordered_map>
#include <mutex>

#include "asio.hpp"

namespace aardvark {

using Callback = std::function<void(void)>;

class EventLoop {
  public:
    int set_timeout(Callback cb, int timeout);
    void clear_timeout(int id);
    int post_callback(Callback cb);
    void cancel_callback(int);
    void execute_callbacks();

  private:
    asio::io_context io = asio::io_context();
    std::unordered_map<int, asio::steady_timer> timers;
    std::mutex timers_mutex;
    int id = 0;
    std::unordered_map<int, Callback> callbacks;
    std::mutex callbacks_mutex;
};

} // namespace aardvark
