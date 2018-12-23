#include <algorithm>
#include "responder.hpp"

namespace aardvark {

template<class T>
bool contains(std::vector<T> vec, T item) {
  return std::find(vec.begin(), vec.end(), item) != vec.end();
};

void ResponderReconciler::reconcile(
    std::vector<std::shared_ptr<Element>> &hit_elements) {
  // TODO
}

/*
  // Call `end` on responders that are no longer hit
  for (auto elem : prev_responder_elements) {
    if (!contains(hit_elements, elem)) {
      elem->get_responder()->end(/ terminate / false);
    }
  }
  std::vector<std::shared_ptr<Element>> responder_elements;
  bool is_captured = false;

  std::vector<ResponderBehaviour> behaviours;
  for (auto elem : hit_elements) {
    auto b = elem->get_responder_behaviour();
    if (b == ResponderBehaviour::Capture) {
      capture_element = elem;
      break;
    }
  }

  for (auto elem : hit_elements) {
    bool was_responder = contains(prev_responder_elements, elem);
    // Call `start` or `update` on responders that are hit, until some of them
    // 'captures', i.e. doesn't pass calls to the next responder in stack.
    if (!is_captured) {
      auto responder = elem->get_responder();
      if (responder != nullptr) {
        is_captured = was_responder ? responder->update() : responder->start();
        responder_elements.push_back(elem);
      }
    } else {
      if (was_responder) {
        auto responder = elem->get_responder();
        if (responder != nullptr) responder->end(/ terminate / true);
      }
    }
  }
  prev_responder_elements = responder_elements;
*/

}  // namespace aardvark
