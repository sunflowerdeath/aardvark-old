#pragma once

#include <memory>
#include <optional>
#include <vector>
#include "element.hpp"

namespace aardvark {

class Element;

enum class ResponderMode : unsigned int {
    // After element handles event, it passes it to the element that is behind.
    PassThrough,

    // Passes event to the parent element (or any further ancestor) behind this
    // element.
    // This is default mode.
    PassToParent,

    // Does not pass event after handling.
    Absorb,

    // Makes this element handle events exclusively and even when it is no
    // longer
    // hit by pointer. Element continues to handle event until it stops
    // capturing
    // or is removed from the document.
    Capture
};

class Responder {
  public:
    // Is called when element starts responding to events.
    virtual void start(){};

    // Is called for each move on the element.
    virtual void update(){};

    // Is called when element stops responding to events.
    // Responding is terminated, when the event is not ended, but some other
    // responder prevented this element from responding, for example,
    // absorbed or captured the event.
    virtual void end(bool is_terminated){};
};

class ResponderReconciler {
  public:
    // Gets stack of elements that are under the pointer, determines which
    // responders should handle event, and calls handers.
    void reconcile(std::vector<std::shared_ptr<Element>>& hit_elements,
                   Element* root_element);

  private:
    std::optional<std::shared_ptr<Element>> capturing_element = std::nullopt;
    std::vector<std::shared_ptr<Element>> prev_active_elements;
};

}  // namespace aardvark
