#include "responder.hpp"
#include <algorithm>
#include <iostream>

namespace aardvark {

template <class T>
bool contains(std::vector<T> vec, T item) {
    return std::find(vec.begin(), vec.end(), item) != vec.end();
};

void ResponderReconciler::reconcile(
    std::vector<std::shared_ptr<Element>>& hit_elements,
    Element* root_element) {

    // If some element was capturing, first check that it is not removed from
    // the document and continues to capture.
    if (capturing_element != std::nullopt) {
        auto elem_val = capturing_element.value();
        if (root_element->is_parent_of(elem_val.get()) &&
            elem_val->get_responder_mode() == ResponderMode::Capture) {
            active_responders.push_back(elem_val->get_responder());
        } else {
            capturing_element = std::nullopt;
        }
    }

    if (capturing_element == std::nullopt) {
        // Iterate elements from top to bottom
        auto it = hit_elements.rbegin();
        while (it != hit_elements.rend()) {
            auto elem = *it;
            active_responders.push_back(elem->get_responder());
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
                // Capturing element becomes only element handling event
                capturing_element = elem;
                active_responders.erase(active_responders.begin(),
                                        active_responders.end() - 1);
                break;
            }
        }
    }

    if (capturing_element == std::nullopt) {
        // End responders that are no longer active
        for (auto responder : prev_active_responders) {
            if (responder == nullptr) continue;
            if (!contains(active_responders, responder)) {
                responder->end(/* is_terminated */ false);
            }
        }
        // Call handlers of active responders
        for (auto responder : active_responders) {
            if (responder == nullptr) continue;
            if (contains(prev_active_responders, responder)) {
                responder->update();
            } else {
                responder->start();
            }
        }
    } else {
        auto capturing_responder = active_responders[0];
        // Terminate all previously active responders, except capturing one
        for (auto responder : prev_active_responders) {
            if (responder == capturing_responder) continue;
            if (responder != nullptr) {
                responder->end(/* is_terminated */ true);
            }
        }
        // Call handler of the capturing responder
        if (capturing_responder != nullptr) {
            if (contains(prev_active_responders, capturing_responder)) {
                capturing_responder->update();
            } else {
                capturing_responder->start();
            }
        }
    }

    prev_active_responders = active_responders;
    active_responders.clear();
}

}  // namespace aardvark
