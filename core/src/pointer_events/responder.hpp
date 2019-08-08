#pragma once

#include <functional>
#include "../events.hpp"

namespace aardvark {

using PointerEventHandler = std::function<void(PointerEvent)>;

// Class that handles pointer events of the element
class Responder {
  public:
    virtual void handler(PointerEvent event, int hz){};
};

}
