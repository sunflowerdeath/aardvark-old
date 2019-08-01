#include "pointer_event_manager.hpp"

namespace aardvark {

template <class K, class V>
bool map_contains(const std::map<K, V>& map, const K& key) {
    return map.find(key) != map.end();
}

// template <class T>
// void vec_erase(const std::vector<T>& vec, const T& elem) {
    // auto it = std::find(vec.begin(), vec.end(), elem);
    // if (it != vec.end()) vec.erase(it);
// }

PointerEventManager::PointerEventManager(Document* document)
    : document(document) {
    hit_tester = std::make_unique<HitTester>(document);
    reconciler = std::make_unique<ResponderReconciler>(document);
}

void PointerEventManager::handle_event(PointerEvent event) {
    // call before handlers
    for (auto& handler : before_handlers) handler(event);

    // call elements responders handlers
    hit_tester->test(event.left, event.top);
    reconciler->reconcile(event, hit_tester->hit_elements);

    // call tracked pointers handlers
    for (auto& handler : tracked_pointers_handlers[event.pointer_id]) {
        handler(event);
    }

    // call after handlers
    for (auto& handler : after_handlers) handler(event);

    if (event.action == PointerAction::pointer_up) {
        // remove list of tracked pointers handlers
        tracked_pointers_handlers.erase(event.pointer_id);
    }
}

void PointerEventManager::add_handler(PointerEventHandler handler, bool after) {
    auto handlers = after ? after_handlers : before_handlers;
    handlers.push_back(handler);
}

void PointerEventManager::remove_handler(PointerEventHandler handler, bool after) {
    auto handlers = after ? after_handlers : before_handlers;
    // TODO
    // vec_erase(handlers, handler);
}

void PointerEventManager::start_tracking_pointer(int pointer_id,
                                            PointerEventHandler handler) {
    if (!map_contains(tracked_pointers_handlers, pointer_id)) {
        tracked_pointers_handlers[pointer_id] =
            std::vector<PointerEventHandler>();
    }
    tracked_pointers_handlers[pointer_id].push_back(handler);
}

void PointerEventManager::stop_tracking_pointer(int pointer_id,
                                           PointerEventHandler handler) {
    if (map_contains(tracked_pointers_handlers, pointer_id)) {
        // TODO
        // vec_erase(tracked_pointers_handlers[pointer_id], handler);
    }
}

}
