#include "align.hpp"

namespace aardvark::elements {

Align::Align(std::shared_ptr<Element> child, AlignmentValue left,
             AlignmentValue top, AlignmentValue right, AlignmentValue bottom,
             bool is_repaint_boundary)
    : SingleChildElement(child, is_repaint_boundary,
                         /* size_depends_on_parent */ true),
      left(left),
      top(top),
      right(right),
      bottom(bottom){};

float calc_value(AlignmentValue val, float total) {
    return std::holds_alternative<value::abs>(val)
               ? std::get<value::abs>(val).val
               : (std::get<value::rel>(val).val * total);
}

Size Align::layout(BoxConstraints constraints) {
    bool has_left = !std::holds_alternative<value::none>(left);
    bool has_top = !std::holds_alternative<value::none>(top);

    int horiz = calc_value(has_left ? left : right, constraints.max_width);
    int vert = calc_value(has_top ? top : bottom, constraints.max_height);

    auto child_constraints = BoxConstraints{
        0,                              // min_width
        constraints.max_width - horiz,  // max_width
        0,                              // min_height
        constraints.max_height - vert   // max_height
    };
    auto size = document->layout_element(child.get(), child_constraints);
    child->size = size;
    child->rel_position = Position{
        has_left ? horiz
                 : (constraints.max_width - horiz - size.width),        // left
        has_top ? vert : (constraints.max_height - vert - size.height)  // top
    };
    return constraints.max_size();
};

void Align::paint(bool is_changed) { document->paint_element(child.get()); };

}  // namespace aardvark::elements
