#pragma once

#include <functional>
#include "../events.hpp"

namespace aardvark {

using PointerEventHandler = std::function<void(PointerEvent)>;

// Class that handles pointer events of the element
class Responder {
  public:
    virtual void pointer_add(PointerEvent event){};
    virtual void pointer_update(PointerEvent event){};
    virtual void pointer_remove(PointerEvent event){};
};

}
