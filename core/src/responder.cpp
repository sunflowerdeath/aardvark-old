#include <iostream>
#include <algorithm>
#include "responder.hpp"

namespace aardvark {

template<class T>
bool contains(std::vector<T> vec, T item) {
    return std::find(vec.begin(), vec.end(), item) != vec.end();
};

void ResponderReconciler::reconcile(
    std::vector<std::shared_ptr<Element>>& hit_elements,
    Element* root_element) {

    // Determine elements that may handle current event
    std::vector<std::shared_ptr<Element>> active_elements;

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
      // Iterate elements from top to bottom
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
              // Capturing element becomes only element handling event
              capturing_element = elem;
              active_elements.erase(active_elements.begin(),
                                    active_elements.end() - 1);
              break;
          }
      }
    }

    if (capturing_element == std::nullopt) {
      // End responders that are no longer active
      for (auto elem : prev_active_elements) {
        auto responder = elem->get_responder();
        if (responder == nullptr) continue;
        if (!contains(active_elements, elem)) {
            responder->end(/* is_terminated */ false);
        }
      }
      // Call handlers of active responders
      for (auto elem : active_elements) {
          auto responder = elem->get_responder();
          if (responder == nullptr) continue;
          if (contains(prev_active_elements, elem)) {
              responder->update();
          } else {
              responder->start();
          }
      }
    } else {
      // Terminate previously active responders
      for (auto elem : prev_active_elements) {
          if (elem == capturing_element) continue;
          auto responder = elem->get_responder();
          if (responder != nullptr) {
              responder->end(/* is_terminated */ true);
          }
      }
      // Call handler of the capturing responder
      auto responder = capturing_element.value()->get_responder();
      if (responder != nullptr) {
          if (contains(prev_active_elements, capturing_element.value())) {
              responder->update();
          } else {
              responder->start();
          }
      }
    }
    
    prev_active_elements = active_elements;
}

}  // namespace aardvark
