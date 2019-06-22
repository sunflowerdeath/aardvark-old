#include "responder.hpp"
#include <algorithm>
#include <iostream>

namespace aardvark {

template <class T>
bool contains(std::vector<T> vec, T item) {
    return std::find(vec.begin(), vec.end(), item) != vec.end();
};

void ResponderReconciler::reconcile(
    const PointerEvent& event,
    std::vector<std::shared_ptr<Element>>& hit_elements,
    Element* root_element) {
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

    // If some element was capturing, first check that it is not removed from
    // the document and continues to capture.
    if (capturing_element != std::nullopt) {
        auto elem_val = capturing_element.value();
        if (root_element->is_parent_of(elem_val.get()) &&
            elem_val->get_responder_mode() == ResponderMode::Capture) {
            active_elements.push_back(elem_val);
        } else {
            capturing_element = std::nullopt;
        }
    }

    if (capturing_element == std::nullopt) {
        // Iterate hit elements from top to bottom
        auto it = hit_elements.rbegin();
        while (it != hit_elements.rend()) {
            auto elem = *it;
            active_elements.push_back(elem);
            auto mode = elem->get_responder_mode();
            if (mode == ResponderMode::PassThrough) {
                it++;  // Pass event handling to the next element
            } else if (mode == ResponderMode::PassToParent) {
                // Pass to element that is parent of passing
                it++;
                while (it != hit_elements.rend()) {
                    if ((*it)->is_parent_of(elem.get())) break;
                    it++;
                }
            } else if (mode == ResponderMode::Absorb) {
                break;  // Do not pass event handling
            } else if (mode == ResponderMode::Capture) {
                // Capturing element becomes only element that handles event
                capturing_element = elem;
                active_elements.erase(active_elements.begin(),
                                      active_elements.end() - 1);
                break;
            }
        }
    }

    auto active_responders = std::vector<Responder*>();
    for (auto& elem : active_elements) {
        auto responder = elem->get_responder();
        if (responder != nullptr) active_responders.push_back(responder);
    }

    auto is_termination = event.action == PointerEvent::Action::pointer_up;

    if (capturing_element == std::nullopt) {
        if (is_termination) {
            // End all responders
            for (auto responder : current_pointer->active_responders) {
                responder->end(event, /* is_terminated */ false);
            }
        } else {
            // Call `end` handlers of responders that are no longer active
            for (auto responder : current_pointer->active_responders) {
                if (!contains(active_responders, responder)) {
                    responder->end(event, /* is_terminated */ false);
                }
            }
            // Call `start` or `update` handlers of active responders
            for (auto responder : active_responders) {
                if (contains(current_pointer->active_responders, responder)) {
                    responder->update(event);
                } else {
                    responder->start(event);
                }
            }
        }
    } else {
        auto capturing_responder = //capturing_element->get_responder();
            active_responders.size() > 0 ? active_responders[0] : nullptr;
        if (capturing_responder != nullptr) {
            // Terminate all previously active responders, except capturing one
            for (auto& pointer_it : pointers) {
                for (auto& responder : pointer_it.second.active_responders) {
                    if (responder != capturing_responder) {
                        responder->end(event, /* is_terminated */ true);
                    }
                }
            }
            // Call handler of the capturing responder
            if (contains(current_pointer->active_responders,
                         capturing_responder)) {
                capturing_responder->update(event);
            } else {
                capturing_responder->start(event);
            }
        }
    }

    if (is_termination) {
        pointers.erase(event.pointer_id);
    } else {
        current_pointer->active_responders = active_responders;
    }
}

}  // namespace aardvark
