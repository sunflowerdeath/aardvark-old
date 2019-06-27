#pragma once

#include <memory>
#include <optional>
#include <vector>
#include <unordered_map>
#include "element.hpp"
#include "events.hpp"

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
    virtual void start(PointerEvent event){};

    // Is called for each move on the element.
    virtual void update(PointerEvent event){};

    // Is called when element stops responding to events.
    // Responding is terminated, when the event is not ended, but some other
    // responder prevented this element from responding, for example,
    // absorbed or captured the event.
    virtual void end(PointerEvent event, bool is_terminated){};
};

// Structure that stores list of responders of the single pointer
struct PointerResponders {
    std::vector<std::shared_ptr<Element>> active_elements;
    std::vector<std::shared_ptr<Element>> prev_active_elements;
    std::vector<Responder*> active_responders;
    std::vector<Responder*> prev_active_responders;
};

class ResponderReconciler {
  public:
    // Takes stack of elements that are under the pointer, determines which
    // responders should handle event and calls their handers.
    void reconcile(const PointerEvent& event,
                   std::vector<std::shared_ptr<Element>>& hit_elements,
                   Element* root_element);

  private:
    std::optional<std::shared_ptr<Element>> capturing_element = std::nullopt;
    std::unordered_map<int, PointerResponders> pointers;
};

}  // namespace aardvark
