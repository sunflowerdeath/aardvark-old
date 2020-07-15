#include "elements/aligned.hpp"

namespace aardvark {

AlignedElement::AlignedElement(
    std::shared_ptr<Element> child,
    Alignment alignment,
    bool adjust_child_size,
    bool is_repaint_boundary)
    : SingleChildElement(
          std::move(child),
          is_repaint_boundary,
          /* size_depends_on_parent */ true),
      adjust_child_size(adjust_child_size),
      alignment(alignment){};

float AlignedElement::get_intrinsic_height(float width) {
    auto align = alignment.vert.calc(0);
    return align + child->query_intrinsic_height(
                       width - (adjust_child_size ? align : 0));
}

float AlignedElement::get_intrinsic_width(float height) {
    auto align = alignment.horiz.calc(0);
    return align + child->query_intrinsic_width(
                       height - (adjust_child_size ? align : 0));
}

Size AlignedElement::layout(BoxConstraints constraints) {
    auto horiz = alignment.horiz.calc(constraints.max_width);
    auto vert = alignment.vert.calc(constraints.max_height);

    auto child_constraints = BoxConstraints{
        0,                                                        // min_width
        constraints.max_width - (adjust_child_size ? horiz : 0),  // max_width
        0,                                                        // min_height
        constraints.max_height - (adjust_child_size ? vert : 0)   // max_height
    };
    auto size = document->layout_element(child.get(), child_constraints);
    child->size = size;
    auto left = (alignment.origin == AlignmentOrigin::top_left ||
                 alignment.origin == AlignmentOrigin::bottom_left)
                    ? horiz
                    : (constraints.max_width - horiz - size.width);
    auto top = (alignment.origin == AlignmentOrigin::top_left ||
                alignment.origin == AlignmentOrigin::top_right)
                   ? vert
                   : (constraints.max_height - vert - size.height);
    child->rel_position = Position{left, top};
    return constraints.max_size();
};

void AlignedElement::paint(bool is_changed) {
    document->paint_element(child.get());
};

}  // namespace aardvark
