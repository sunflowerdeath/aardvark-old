#pragma once

#include <memory>
#include <map>
#include <vector>
#include <nod/nod.hpp>
#include "../document.hpp"
#include "hit_tester.hpp"
#include "responder.hpp"
#include "responder_reconciler.hpp"

namespace aardvark {

class Element;
class Document;
class HitTester;
class ResponderReconciler;

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

    void handle_event(const PointerEvent& event);

  private:
    Document* document;
    std::unique_ptr<HitTester> hit_tester;
    std::unique_ptr<ResponderReconciler> reconciler;
    nod::signal<void(const PointerEvent&)> before_signal;
    nod::signal<void(const PointerEvent&)> after_signal;
    std::map<int, nod::signal<void(const PointerEvent&)>> pointers_signals;
};

}
