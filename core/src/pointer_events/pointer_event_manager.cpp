#include "pointer_event_manager.hpp"

namespace aardvark {

template <class K, class V>
bool map_contains(const std::map<K, V>& map, const K& key) {
    return map.find(key) != map.end();
}

template <class T>
bool vec_contains(const std::vector<T>& vec, T item) {
    return std::find(vec.begin(), vec.end(), item) != vec.end();
};

PointerEventManager::PointerEventManager(Document* document)
    : document(document) {
    hit_tester = std::make_unique<HitTester>(document);
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

void PointerEventManager::store_hit_elems(const PointerEvent& event) {
    hit_elems[const_cast<PointerEvent*>(&event)] =
        hit_tester->test(event.left, event.top);
}

void PointerEventManager::clear_hit_elems() {
    hit_elems.clear();
}

void PointerEventManager::handle_event(const PointerEvent& event) {
    before_signal(event);

    call_responders_handlers(event,
                             hit_elems[const_cast<PointerEvent*>(&event)]);

    if (map_contains(pointers_signals, event.pointer_id)) {
        pointers_signals[event.pointer_id](event);
    }

    after_signal(event);

    if (event.action == PointerAction::pointer_up) {
        // remove pointer signal
        pointers_signals.erase(event.pointer_id);
    }
}

void PointerEventManager::call_responders_handlers(
    const PointerEvent& event,
    const std::vector<std::weak_ptr<Element>>& hit_elems) {

    std::vector<std::weak_ptr<Element>>* pointer_prev_hit_elems;
    auto it = prev_hit_elems.find(event.pointer_id);
    if (it == prev_hit_elems.end()) {
        prev_hit_elems[event.pointer_id] =
            std::vector<std::weak_ptr<Element>>();
        pointer_prev_hit_elems = &prev_hit_elems[event.pointer_id];
    } else {
        pointer_prev_hit_elems = &it->second;
    }

    auto is_pointer_remove = event.action == PointerAction::pointer_up;
    if (is_pointer_remove) {
        // End all responders
        for (auto elem_wptr : *pointer_prev_hit_elems) {
            if (auto elem = elem_wptr.lock()) {
                elem->get_responder()->handler(event,
                                               ResponderEventType::remove);
            }
        }
    } else {
        // Call `remove` handlers of responders that are no longer active
        for (auto elem_wptr : *pointer_prev_hit_elems) {
            if (auto elem = elem_wptr.lock()) {
                if (!vec_contains(hit_elems, elem_wptr)) {
                    elem->get_responder()->handler(event,
                                                   ResponderEventType::remove);
                }
            }
        }
        // Call `add` and `update` handlers
        for (auto elem_wptr : hit_elems) {
            if (auto elem = elem_wptr.lock()) {
                auto event_type =
                    vec_contains(*pointer_prev_hit_elems, elem_wptr)
                        ? ResponderEventType::update
                        : ResponderEventType::add;
                elem->get_responder()->handler(event, event_type);
            }
        }
    }

    if (is_pointer_remove) {
        prev_hit_elems.erase(event.pointer_id);
    } else {
        prev_hit_elems[event.pointer_id] = hit_elems;
    }
};

}  // namespace aardvark
