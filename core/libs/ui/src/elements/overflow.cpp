#include "elements/overflow.hpp"

namespace aardvark {

float OverflowConstraint::resolve(float original) {
    if (type == OverflowType::unconstrained) {
        return std::numeric_limits<float>::infinity();
    } else if (type == OverflowType::original) {
        return original;
    } else {
        // sized
        return size;
    }
}

OverflowConstraint OverflowConstraint::original =
    OverflowConstraint{OverflowType::original};

OverflowConstraint OverflowConstraint::unconstrained =
    OverflowConstraint{OverflowType::unconstrained};

OverflowConstraint OverflowConstraint::sized(float size) {
    return OverflowConstraint{OverflowType::sized, size};
}

Size OverflowElement::layout(BoxConstraints constraints) {
    auto child_constraints = BoxConstraints{
        constraints.min_width,                      // min_width
        max_width.resolve(constraints.max_width),   // max_width
        constraints.min_height,                     // min_height
        max_height.resolve(constraints.max_height)  // max_height
    };
    auto child_size = child->layout(child_constraints);
    child->size = child_size;
    child->rel_position = Position{0, 0};
    return child_size;
}

}  // namespace aardvark
