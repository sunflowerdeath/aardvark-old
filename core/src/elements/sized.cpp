#include "sized.hpp"

namespace aardvark::elements {

Sized::Sized(std::shared_ptr<Element> child, SizeConstraints size_constraints,
                         bool is_repaint_boundary)
    : SingleChildElement(child, is_repaint_boundary,
                         /* size_depends_on_parent */ false),
      size_constraints(size_constraints){};

Size Sized::layout(BoxConstraints constraints) {
    auto child_constraints = BoxConstraints{
        size_constraints.min_width.calc(constraints.max_width,
                                        constraints.min_width),  // min_width
        size_constraints.max_width.calc(constraints.max_width,
                                        constraints.max_width),  // max_width
        size_constraints.min_height.calc(constraints.max_height,
                                         constraints.min_height),  // min_height
        size_constraints.max_height.calc(
            constraints.max_height, constraints.max_height)};  // max_height
    auto child_size = document->layout_element(child.get(), child_constraints);
    child->size = child_size;
    child->rel_position = Position{0 /* left */, 0 /* top */};
    return child_size;
};

void Sized::paint(bool is_changed) {
    document->paint_element(child.get());
};

}  // namespace aardvark::elements
