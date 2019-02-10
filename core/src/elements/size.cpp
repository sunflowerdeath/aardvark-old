#include "fixed_size.hpp"

namespace aardvark::elements {

Size::Size(std::shared_ptr<Element> child, Value min_width, Value max_width,
           Value min_height, Value max_height, bool is_repaint_boundary = false)
    : SingleChildElement(child, is_repaint_boundary,
                         /* size_depends_on_parent */ false),
      min_width(min_width),
      max_width(max_width),
      min_height(min_height),
      max_height(max_height){};

Size Size::layout(BoxConstraints constraints) {
    auto constraints = BoxConstraints{
        min_width.calc(constraints.max_width, constraints.min_width),
        max_width.calc(constraints.max_width, constraints.max_width),
        min_width.calc(constraints.max_height, constraints.min_height),
        max_width.calc(constraints.max_height, constraints.max_height)};
    auto size = document->layout_element(child.get(), constraints);
    child->size = size;
    child->rel_position = Position{0 /* left */, 0 /* top */};
    return size;
};

void Size::paint(bool is_changed) {
    document->paint_element(child.get());
};

}  // namespace aardvark::elements
