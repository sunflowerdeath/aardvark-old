#include "elements/sized.hpp"

namespace aardvark {

float SizedElement::get_intrinsic_height(float width) {
    auto res = size_constraints.height.is_none()
                   ? child->query_intrinsic_height(width)
                   : size_constraints.height.calc(0);
    if (!size_constraints.min_height.is_none()) {
        res = fmax(res, size_constraints.min_height.calc(0));
    }
    if (!size_constraints.max_height.is_none()) {
        res = fmin(res, size_constraints.max_height.calc(0));
    }
    return res;
}

float SizedElement::get_intrinsic_width(float height) {
    auto res = size_constraints.width.is_none()
                   ? child->query_intrinsic_width(height)
                   : size_constraints.width.calc(0);
    if (!size_constraints.min_width.is_none()) {
        res = fmax(res, size_constraints.min_width.calc(0));
    }
    if (!size_constraints.max_width.is_none()) {
        res = fmin(res, size_constraints.max_width.calc(0));
    }
    return res;
}

float calc_min(const std::array<Value, 2>& values, float parent_min,
               float parent_max) {
    auto res = parent_min; // TODO option to allow underflow
    for (auto val : values) {
        if (!val.is_none()) res = fmax(val.calc(parent_max), res);
    }
    return res;
}

float calc_max(const std::array<Value, 2>& values, float parent_max) {
    auto res = parent_max; // TODO option to allow overflow
    for (auto val : values) {
        if (!val.is_none()) res = fmin(val.calc(parent_max), res);
    }
    return res;
}

Size SizedElement::layout(BoxConstraints constraints) {
    auto& parent = constraints;
    auto& self = size_constraints;
    auto min_width = calc_min({self.min_width, self.width}, parent.min_width,
                              parent.max_width);
    auto max_width = calc_max({self.max_width, self.width}, parent.max_width);
    auto min_height = calc_min({self.min_height, self.height}, parent.min_width,
                               parent.max_height);
    auto max_height =
        calc_max({self.max_height, self.height}, parent.max_height);
    auto child_constraints =
        BoxConstraints{min_width, max_width, min_height, max_height};
    auto child_size = document->layout_element(child.get(), child_constraints);
    child->size = child_size;
    child->rel_position = Position{0 /* left */, 0 /* top */};
    return Size{
        fmin(fmax(child_size.width, min_width), max_width),    // width
        fmin(fmax(child_size.height, min_height), max_height)  // height
    };
}

}  // namespace aardvark::elements
