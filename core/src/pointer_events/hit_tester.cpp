#include "hit_tester.hpp"

namespace aardvark {

void HitTester::test(float left, float top) {
    transform.reset();
    hit_elements.clear();
    test_element(document->root, left, top);
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
            hit_elements.push_back(elem);
        }
        elem->visit_children(std::bind(&HitTester::test_element, this,
                                       std::placeholders::_1, left, top));
    }
    
    transform.set9(prev_transform);
};

}  // namespace aardvark
