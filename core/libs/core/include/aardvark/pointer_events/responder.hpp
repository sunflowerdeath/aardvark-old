#pragma once

#include <functional>
#include "../events.hpp"

namespace aardvark {

using PointerEventHandler = std::function<void(PointerEvent)>;

enum class ResponderEventType { add, remove, update };

// Class that handles pointer events of the element
class Responder {
  public:
    virtual void handler(PointerEvent event, ResponderEventType event_type){};
};

}
