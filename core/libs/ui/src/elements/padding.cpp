#include "elements/padding.hpp"

namespace aardvark {

float PaddingElement::get_intrinsic_height() {
    return padding.top + padding.bottom + child->get_intrinsic_height();
}

float PaddingElement::get_intrinsic_width() {
    return padding.left + padding.right + child->get_intrinsic_width();
}

Size PaddingElement::layout(BoxConstraints constraints) {
    auto horiz = padding.left + padding.right;
    auto vert = padding.top + padding.bottom;
    auto child_constraints = BoxConstraints{
        constraints.min_width,          // min_width
        constraints.max_width - horiz,  // max_width
        constraints.min_height,         // min_height
        constraints.max_height - vert   // max_height
    };
    auto child_size = document->layout_element(child.get(), child_constraints);
    child->size = child_size;
    child->rel_position = Position{padding.left, padding.top};
    return Size{child_size.width + horiz, child_size.height + vert};
}

}  // namespace aardvark
