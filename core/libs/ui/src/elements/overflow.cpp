#include "elements/overflow.hpp"

namespace aardvark {

Size OverflowElement::layout(BoxConstraints constraints) {
    auto child_constraints = BoxConstraints{
        constraints.min_width,  // min_width
        max_width.has_value() ? max_width.value()
                              : constraints.max_width,  // max_width
        constraints.min_height,                         // min_height
        max_height.has_value() ? max_height.value()
                               : constraints.max_height  // max_height
    };
    auto child_size = child->layout(child_constraints);
    child->size = child_size;
    child->rel_position = Position{0, 0};
    return child_size;
}

}  // namespace aardvark
