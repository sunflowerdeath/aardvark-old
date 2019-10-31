#include "hit_tester.hpp"

namespace aardvark {

std::vector<std::weak_ptr<Element>> HitTester::test(float left, float top) {
    test_element(document->root, left, top);

    auto hit_elements = std::vector<std::shared_ptr<Element>>();
    // Iterate elements from top to bottom
    auto it = elements_under_pointer.rbegin();
    while (it != elements_under_pointer.rend()) {
        auto elem = *it;
        hit_elements.push_back(elem);
        auto mode = elem->get_hit_test_mode();
        if (mode == HitTestMode::PassThrough) {
            it++;  // Pass to the next element
        } else if (mode == HitTestMode::PassToParent) {
            // Pass to next element that is parent of passing
            it++;
            while (it != elements_under_pointer.rend()) {
                if ((*it)->is_parent_of(elem.get())) break;
                it++;
            }
        } else if (mode == HitTestMode::Absorb) {
            break;  // Do not pass event handling
        }
    }

    auto hit_elements_with_responders = std::vector<std::weak_ptr<Element>>();
    for (auto elem : hit_elements) {
        if (elem->get_responder() != nullptr) {
            hit_elements_with_responders.push_back(elem);
        }
    }

    transform.reset();
    elements_under_pointer.clear();

    return hit_elements_with_responders;
}

void HitTester::test_element(std::shared_ptr<Element> elem, float left,
                             float top) {
    SkScalar prev_transform[9];
    transform.get9(prev_transform);

    transform.postTranslate(-elem->rel_position.left, -elem->rel_position.top);

    bool clipped = false;
    if (elem->clip != std::nullopt) {
        // Element's clip is relative to element's position
        auto transformed = SkPoint{left, top};
        transform.mapPoints(&transformed, 1);
        clipped =
            !elem->clip.value().contains(transformed.x(), transformed.y());
    }

    if (!clipped) {
        if (elem->is_repaint_boundary) {
            SkMatrix inverse;
            auto inverted = elem->layer_tree->transform.invert(&inverse);
            if (inverted) transform.postConcat(inverse);
        }
        auto rel_pos = SkPoint{left, top};
        transform.mapPoints(&rel_pos, 1);
        if (elem->get_hit_test_mode() != HitTestMode::Disabled &&
            elem->hit_test(rel_pos.x(), rel_pos.y())) {
            elements_under_pointer.push_back(elem);
        }
        elem->visit_children(std::bind(&HitTester::test_element, this,
                                       std::placeholders::_1, left, top));
    }
    
    transform.set9(prev_transform);
};

}  // namespace aardvark
