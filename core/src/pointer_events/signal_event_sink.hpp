#pragma once

#include <functional>
#include <nod/nod.hpp>

template<typename T>
class SignalEventSink {
  public:
    using EventHandler = std::function<void(const T&)>;
    
    nod::connection add_handler(const EventHandler& handler) {
        return signal.connect(handler);
    }

    void handle_event(const T& event) { signal(event); }

  private:
    nod::signal<void(const T&)> signal;
};
