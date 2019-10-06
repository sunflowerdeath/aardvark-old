#include "pointer_event_manager.hpp"

namespace aardvark {

template <class K, class V>
bool map_contains(const std::map<K, V>& map, const K& key) {
    return map.find(key) != map.end();
}

PointerEventManager::PointerEventManager(Document* document)
    : document(document) {
    hit_tester = std::make_unique<HitTester>(document);
    reconciler = std::make_unique<ResponderReconciler>(document);
}

void PointerEventManager::handle_event(const PointerEvent& event) {
    before_signal(event);

    // call elements responders handlers
    hit_tester->test(event.left, event.top);
    reconciler->reconcile(event, hit_tester->hit_elements);

    if (map_contains(pointers_signals, event.pointer_id)) {
        pointers_signals[event.pointer_id](event);
    }

    after_signal(event);

    if (event.action == PointerAction::pointer_up) {
        // remove pointer signal
        pointers_signals.erase(event.pointer_id);
    }
}

nod::connection PointerEventManager::add_handler(
    const PointerEventHandler& handler, const bool after_elements) {
    auto& signal = after_elements ? after_signal : before_signal;
    return signal.connect(handler);
}

nod::connection PointerEventManager::start_tracking_pointer(
    const int pointer_id, const PointerEventHandler& handler) {
    if (!map_contains(pointers_signals, pointer_id)) {
        pointers_signals[pointer_id] = nod::signal<void(const PointerEvent&)>();
    }
    return pointers_signals[pointer_id].connect(handler);
}
}  // namespace aardvark
