#include "elements/translated.hpp"

namespace aardvark {

float TranslatedElement::get_intrinsic_height(float width) {
    auto child_height = child->get_intrinsic_height(width);
    return child_height + translation.top.calc(child_height);
}

float TranslatedElement::get_intrinsic_width(float height) {
    auto child_width = child->get_intrinsic_width(height);
    return child_width + translation.left.calc(child_width);
}

Size TranslatedElement::layout(BoxConstraints constraints) {
    auto child_size = document->layout_element(child.get(), constraints);
    child->size = child_size;
    auto left_tr = translation.left.calc(child_size.width);
    auto top_tr = translation.top.calc(child_size.height);
    child->rel_position = Position{left_tr, top_tr};
    return Size{/* width */ fmaxf(0, child_size.width + left_tr),
                /* height */ fmaxf(0, child_size.height + top_tr)};
}

bool TranslatedElement::hit_test(double left, double top) {
    auto left_tr = translation.left.calc(size.width);
    auto top_tr = translation.left.calc(size.width);
    return (abs_position.left >= left_tr &&
            abs_position.left <= left_tr + size.width &&
            abs_position.top >= top_tr &&
            abs_position.top <= top_tr + size.height);
}

}  // namespace aardvark
