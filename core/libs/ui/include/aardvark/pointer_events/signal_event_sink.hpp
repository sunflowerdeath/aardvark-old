#pragma once

#include <functional>
#include <nod/nod.hpp>

#include "../base_types.hpp"

namespace aardvark {

template<typename T>
class SignalEventSink {
  public:
    using EventHandler = std::function<void(const T&)>;
    
    nod::connection add_handler(const EventHandler& handler) {
        return signal.connect(handler);
    }

    std::shared_ptr<Connection> add_handler2(const EventHandler& handler) {
        return std::make_shared<NodConnection>(signal.connect(handler));
    }

    void handle_event(const T& event) { signal(event); }

  private:
    nod::signal<void(const T&)> signal;
};

}  // namespace aardvark
