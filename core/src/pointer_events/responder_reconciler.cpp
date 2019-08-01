#include "responder_reconciler.hpp"
#include <algorithm>
#include <iostream>

namespace aardvark {

template <class T>
bool contains(std::vector<T> vec, T item) {
    return std::find(vec.begin(), vec.end(), item) != vec.end();
};

void ResponderReconciler::reconcile(
    const PointerEvent& event,
    const std::vector<std::shared_ptr<Element>>& hit_elements) {
    // Find or create current pointer
    auto current_pointer_it = pointers.find(event.pointer_id);
    PointerResponders* current_pointer;
    if (current_pointer_it == pointers.end()) {
        pointers[event.pointer_id] = PointerResponders();
        current_pointer = &pointers[event.pointer_id];
    } else {
        current_pointer = &current_pointer_it->second;
    }

    auto active_elements = std::vector<std::shared_ptr<Element>>();

    // Iterate hit elements from top to bottom
    auto it = hit_elements.rbegin();
    while (it != hit_elements.rend()) {
        auto elem = *it;
        active_elements.push_back(elem);
        auto mode = elem->get_hit_test_mode();
        if (mode == HitTestMode::PassThrough) {
            it++;  // Pass event handling to the next element
        } else if (mode == HitTestMode::PassToParent) {
            // Pass to element that is parent of passing
            it++;
            while (it != hit_elements.rend()) {
                if ((*it)->is_parent_of(elem.get())) break;
                it++;
            }
        } else if (mode == HitTestMode::Absorb) {
            break;  // Do not pass event handling
        }
    }

    auto active_responders = std::vector<Responder*>();
    for (auto& elem : active_elements) {
        auto responder = elem->get_responder();
        if (responder != nullptr) active_responders.push_back(responder);
    }

    auto is_pointer_remove = event.action == PointerAction::pointer_up;

    if (is_pointer_remove) {
        // End all responders
        for (auto responder : current_pointer->active_responders) {
            responder->pointer_remove(event);
        }
    } else {
        // Call `remove` handlers of responders that are no longer active
        for (auto responder : current_pointer->active_responders) {
            if (!contains(active_responders, responder)) {
                responder->pointer_remove(event);
            }
        }
        // Call `add` or `update` handlers of active responders
        for (auto responder : active_responders) {
            if (contains(current_pointer->active_responders, responder)) {
                responder->pointer_update(event);
            } else {
                responder->pointer_add(event);
            }
        }
    }

    if (is_pointer_remove) {
        pointers.erase(event.pointer_id);
    } else {
        current_pointer->active_responders = active_responders;
    }
}

}  // namespace aardvark
