#include "event_loop.hpp"

namespace aardvark {

int EventLoop::set_timeout(Callback cb, int timeout) {
    auto guard = std::lock_guard<std::mutex>(timers_mutex);
    id++;
    // map.emplace() returns a pair of an iterator to the inserted element 
    // and a bool
    auto it = timers.emplace(id, boost::asio::steady_timer(io)).first;
    auto& timer = it->second;
    timer.expires_after(std::chrono::microseconds(timeout));
    timer.async_wait([this, id = id, cb](const boost::system::error_code& e) {
        bool is_cancelled = false;
        {
            auto guard = std::lock_guard<std::mutex>(timers_mutex);
            if (timers.find(id) == timers.end()) is_cancelled = true;
        }
        // Do not lock during call, so it can set/clear timeouts
        if (!is_cancelled) cb();
    });
    return id;
}

void EventLoop::clear_timeout(int id) {
    auto guard = std::lock_guard<std::mutex>(timers_mutex);
    auto it = timers.find(id);
    if (it != timers.end()) {
        it->second.cancel();
        timers.erase(it);
    }
}

int EventLoop::post_callback(Callback callback) {
    auto guard = std::lock_guard<std::mutex>(callbacks_mutex);
    id++;
    callbacks[id] = callback;
    io.post([this, id = id]() {
        Callback cb;
        {
            auto guard = std::lock_guard<std::mutex>(callbacks_mutex);
            auto it = callbacks.find(id);
            if (it != callbacks.end()) cb = it->second;
        }
        // Do not lock during call, so it can post/remove callbacks
        if (cb) cb();
        {
            auto guard = std::lock_guard<std::mutex>(callbacks_mutex);
            callbacks.erase(id);
        }
    });
    return id;
}

void EventLoop::cancel_callback(int id) {
    auto guard = std::lock_guard<std::mutex>(callbacks_mutex);
    auto it = callbacks.find(id);
    if (it != callbacks.end()) callbacks.erase(it);
}

} // namespace aardvark
