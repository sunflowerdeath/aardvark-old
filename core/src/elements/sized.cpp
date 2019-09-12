#include "sized.hpp"

namespace aardvark::elements {

Sized::Sized(std::shared_ptr<Element> child, SizeConstraints size_constraints,
                         bool is_repaint_boundary)
    : SingleChildElement(child, is_repaint_boundary,
                         /* size_depends_on_parent */ false),
      size_constraints(size_constraints){};

float Sized::get_intrinsic_height() {
    return fmax(size_constraints.min_height.calc(0),
                size_constraints.height.calc(0));
}

float Sized::get_intrinsic_width() {
    return fmax(size_constraints.min_width.calc(0),
                size_constraints.width.calc(0));
}

float calc_min(const std::array<Value, 2>& values, float parent_min,
               float parent_max) {
    auto res = parent_min;
    for (auto val : values) {
        if (!val.is_none()) res = fmax(val.calc(parent_max), res);
    }
    return res;
}

float calc_max(const std::array<Value, 2>& values, float parent_max) {
    auto res = parent_max;
    for (auto val : values) {
        if (!val.is_none()) res = fmin(val.calc(parent_max), res);
    }
    return res;
}

Size Sized::layout(BoxConstraints constraints) {
    auto& parent = constraints;
    auto& self = size_constraints;
    auto child_constraints = BoxConstraints{
        calc_min({self.min_width, self.width}, parent.min_width,
                 parent.max_width),
        calc_max({self.max_width, self.width}, parent.max_width),
        calc_min({self.min_height, self.height}, parent.min_width,
                 parent.max_height),
        calc_max({self.max_height, self.height}, parent.max_height)};
    auto child_size = document->layout_element(child.get(), child_constraints);
    child->size = child_size;
    child->rel_position = Position{0 /* left */, 0 /* top */};
    return child_size;
}

void Sized::paint(bool is_changed) {
    document->paint_element(child.get());
};

}  // namespace aardvark::elements
