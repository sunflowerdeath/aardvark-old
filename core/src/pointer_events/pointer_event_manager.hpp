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
    nod::connection add_handler(const PointerEventHandler& handler,
                                const bool after_elements = false);

    // Register handler to track events of the specified pointer
    nod::connection start_tracking_pointer(const int pointer_id,
                                           const PointerEventHandler& handler);

    // Hit testing for all pointer events is performed before calling any
    // handlers, because they may change the document and then hit testing
    // will be incorrect.
    // Event manager remembers hit elements for each event and then
    // dispatches events according to that.
    void clear_hit_elems();
    void store_hit_elems(const PointerEvent& event);
    void handle_event(const PointerEvent& event);

  private:
    Document* document;

    nod::signal<void(const PointerEvent&)> before_signal;
    nod::signal<void(const PointerEvent&)> after_signal;
    std::map<int, nod::signal<void(const PointerEvent&)>> pointers_signals;

    std::unique_ptr<HitTester> hit_tester;
    std::unordered_map<PointerEvent*, std::vector<std::weak_ptr<Element>>>
        hit_elems;
    std::unordered_map<int, std::vector<std::weak_ptr<Element>>> prev_hit_elems;
    void call_responders_handlers(
        const PointerEvent& event,
        const std::vector<std::weak_ptr<Element>>& hit_elems);
};

}
