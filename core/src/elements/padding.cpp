#include "padding.hpp"

namespace aardvark {

float PaddingElement::get_intrinsic_height() {
    auto top = padding.top.calc(0);
    auto bottom = padding.bottom.calc(0);
    return top + bottom + child->get_intrinsic_height();
}

float PaddingElement::get_intrinsic_width() {
    auto left = padding.left.calc(0);
    auto right = padding.right.calc(0);
    return left + right + child->get_intrinsic_width();
}

Size PaddingElement::layout(BoxConstraints constraints) {
    auto left = padding.left.calc(constraints.max_width);
    auto right = padding.right.calc(constraints.max_width);
    auto top = padding.top.calc(constraints.max_height);
    auto bottom = padding.bottom.calc(constraints.max_height);
    auto child_constraints = BoxConstraints{
        constraints.min_width,                 // min_width
        constraints.max_width - left - right,  // max_width
        constraints.min_height,                // min_height
        constraints.max_height - top - bottom  // max_height
    };
    auto child_size = document->layout_element(child.get(), child_constraints);
    child->size = child_size;
    child->rel_position = Position{left, top};
    return Size{child_size.width + left + right,
                child_size.height + top + bottom};
}

}  // namespace aardvark
