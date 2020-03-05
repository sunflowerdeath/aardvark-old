#pragma once

#include <memory>
#include <map>
#include <vector>
#include <nod/nod.hpp>
#include "../document.hpp"
#include "hit_tester.hpp"
#include "responder.hpp"

namespace aardvark {

class Element;
class Document;
class HitTester;

// Class that controls handling of document pointer events
class PointerEventManager {
  public:
    PointerEventManager(Document* document);

    // Registers handler for all pointer events of the document.
    // When `after` is true, handler will be called after all other handlers.
    std::shared_ptr<Connection> add_handler(
        const PointerEventHandler& handler, const bool after_elements = false);

    // Register handler to track events of the specified pointer
    std::shared_ptr<Connection> start_tracking_pointer(
        const int pointer_id, const PointerEventHandler& handler);

    void handle_event(const PointerEvent& event);

  private:
    Document* document;

    nod::signal<void(const PointerEvent&)> before_signal;
    nod::signal<void(const PointerEvent&)> after_signal;
    std::map<int, nod::signal<void(const PointerEvent&)>> pointers_signals;

    std::unique_ptr<HitTester> hit_tester;
    std::unordered_map<int, std::vector<std::weak_ptr<Element>>> prev_hit_elems;
    void call_responders_handlers(const PointerEvent& event);
};

}
