#include "translate.hpp"

namespace aardvark {

float TranslateElement::get_intrinsic_height() {
    auto child_height = child->get_intrinsic_height();
    return child_height + translation.left.calc(child_height);
}

float TranslateElement::get_intrinsic_width() {
    auto child_width = child->get_intrinsic_width();
    return child_width + translation.top.calc(child_width);
}

Size TranslateElement::layout(BoxConstraints constraints) {
    auto child_size = document->layout_element(child.get(), constraints);
    child->size = child_size;
    auto left_tr = translation.left.calc(child_size.width);
    auto top_tr = translation.top.calc(child_size.height);
    child->rel_position = Position{left_tr, top_tr};
    return Size{/* width */ fmax(0, child_size.width + left_tr),
                /* height */ fmax(0, child_size.height + top_tr)};
}

bool TranslateElement::hit_test(double left, double top) {
    auto left_tr = translation.left.calc(size.width);
    auto top_tr = translation.left.calc(size.width);
    return (abs_position.left >= left_tr &&
            abs_position.left <= left_tr + size.width &&
            abs_position.top >= top_tr &&
            abs_position.top <= top_tr + size.height);
}

}  // namespace aardvark
