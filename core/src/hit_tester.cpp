#include "hit_tester.hpp"

namespace aardvark {

void HitTester::test(std::shared_ptr<Element> root, float left, float top) {
    transform.reset();
    hit_elements.clear();
    test_element(root, left, top);
}

void HitTester::test_element(std::shared_ptr<Element> elem, float left,
                             float top) {
    if (elem->is_repaint_boundary) {
        SkMatrix adjusted;
        adjusted.setTranslate(-elem->abs_position.left,
                              -elem->abs_position.top);
        adjusted.postConcat(elem->layer_tree->transform);
        adjusted.postTranslate(elem->abs_position.left, elem->abs_position.top);
        static_cast<void>(adjusted.invert(&adjusted));
        transform = SkMatrix::Concat(adjusted, transform);
    }
    auto transformed = SkPoint{left, top};
    transform.mapPoints(&transformed, 1);

    bool clipped = false;
    if (elem->clip != std::nullopt) {
        SkPath offset_clip;
        elem->clip.value().offset(elem->abs_position.left,
                                  elem->abs_position.top, &offset_clip);
        clipped =
            !offset_clip.contains(transformed.x(), transformed.y());  // TODO
    }

    if (!clipped) {
        if (elem->hit_test(transformed.x(), transformed.y())) {
            hit_elements.push_back(elem);
        }
        elem->visit_children(std::bind(&HitTester::test_element, this,
                                       std::placeholders::_1, left, top));
    }
};
}
