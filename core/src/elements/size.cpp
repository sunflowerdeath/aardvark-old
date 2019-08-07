#include "size.hpp"

namespace aardvark::elements {

SizeElement::SizeElement(std::shared_ptr<Element> child, ASize size,
                         bool is_repaint_boundary)
    : SingleChildElement(child, is_repaint_boundary,
                         /* size_depends_on_parent */ false),
      size(size){};

Size SizeElement::layout(BoxConstraints constraints) {
    auto child_constraints = BoxConstraints{
        size.min_width.calc(constraints.max_width,
                            constraints.min_width),  // min_width
        size.max_width.calc(constraints.max_width,
                            constraints.max_width),  // max_width
        size.min_height.calc(constraints.max_height,
                             constraints.min_height),  // min_height
        size.max_height.calc(constraints.max_height,
                             constraints.max_height)};  // max_height
    auto child_size = document->layout_element(child.get(), child_constraints);
    child->size = child_size;
    child->rel_position = Position{0 /* left */, 0 /* top */};
    return child_size;
};

void SizeElement::paint(bool is_changed) {
    document->paint_element(child.get());
};

}  // namespace aardvark::elements
