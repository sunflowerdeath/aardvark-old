#include "elements/align.hpp"

namespace aardvark {

AlignElement::AlignElement(
    std::shared_ptr<Element> child,
    Alignment insets,
    bool adjust_child_size,
    bool is_repaint_boundary)
    : SingleChildElement(
          std::move(child),
          is_repaint_boundary,
          /* size_depends_on_parent */ true),
      adjust_child_size(adjust_child_size),
      insets(insets){};

float AlignElement::get_intrinsic_height(float width) {
    return insets.calc_vert(0) +
           child->get_intrinsic_height(width - insets.calc_horiz(0));
}

float AlignElement::get_intrinsic_width(float height) {
    return insets.calc_horiz(0) +
           child->get_intrinsic_width(height - insets.calc_horiz(0));
}

Size AlignElement::layout(BoxConstraints constraints) {
    auto left = insets.left.calc(constraints.max_width);
    auto top = insets.top.calc(constraints.max_height);
    auto right = insets.right.calc(constraints.max_width);
    auto bottom = insets.bottom.calc(constraints.max_height);
    auto horiz = left + right;
    auto vert = top + bottom;

    auto child_constraints = BoxConstraints{
        0,  // min_width
        adjust_child_size ? (constraints.max_width - horiz)
                          : constraints.max_width,  // max_width
        0,                                          // min_height
        adjust_child_size ? (constraints.max_height - vert)
                          : constraints.max_height,  // max_height
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

void AlignElement::paint(bool is_changed) {
    document->paint_element(child.get());
};

}  // namespace aardvark
