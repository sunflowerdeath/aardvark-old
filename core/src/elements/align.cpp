#include "align.hpp"

namespace aardvark::elements {

Align::Align(std::shared_ptr<Element> child, EdgeInsets insets,
             bool is_repaint_boundary)
    : SingleChildElement(child, is_repaint_boundary,
                         /* size_depends_on_parent */ true),
      insets(insets){};

Size Align::layout(BoxConstraints constraints) {
    auto left = insets.left.calc(constraints.max_width);
    auto right = insets.right.calc(constraints.max_width);
    auto top = insets.top.calc(constraints.max_height);
    auto bottom = insets.bottom.calc(constraints.max_height);
    auto horiz = left + right;
    auto vert = top + bottom;

    auto child_constraints = BoxConstraints{
        0,                              // min_width
        constraints.max_width - horiz,  // max_width
        0,                              // min_height
        constraints.max_height - vert   // max_height
    };
    auto size = document->layout_element(child.get(), child_constraints);
    child->size = size;
    child->rel_position = Position{
        insets.left.is_none() ? (constraints.max_width - horiz - size.width)
                              : left,  // left
        insets.top.is_none() ? (constraints.max_height - vert - size.height)
                             : top  // top
    };
    return constraints.max_size();
};

void Align::paint(bool is_changed) { document->paint_element(child.get()); };

}  // namespace aardvark::elements
