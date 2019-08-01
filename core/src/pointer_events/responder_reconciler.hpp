#pragma once

#include <memory>
#include <optional>
#include <vector>
#include <unordered_map>
#include "../document.hpp"
#include "../element.hpp"
#include "../events.hpp"
#include "responder.hpp"

namespace aardvark {

class Document;
class Element;

// Structure that stores responders of the single pointer
struct PointerResponders {
    std::vector<std::shared_ptr<Element>> active_elements;
    std::vector<std::shared_ptr<Element>> prev_active_elements;
    std::vector<Responder*> active_responders;
    std::vector<Responder*> prev_active_responders;
};

// Class that calls event handlers of hit elements responders
class ResponderReconciler {
  public:
    ResponderReconciler(Document* document) : document(document){};

    // Takes stack of elements that are under the pointer, determines which
    // responders should handle event and calls their handers.
    void reconcile(const PointerEvent& event,
                   const std::vector<std::shared_ptr<Element>>& hit_elements);

  private:
    Document* document;
    std::optional<std::shared_ptr<Element>> capturing_element = std::nullopt;
    std::unordered_map<int, PointerResponders> pointers;
};

}  // namespace aardvark
