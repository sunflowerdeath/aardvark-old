#include "elements/aligned.hpp"

namespace aardvark {

Position positional_align(Size container, Size box, Alignment alignment) {
    auto horiz = alignment.horiz.calc(container.width);
    auto vert = alignment.vert.calc(container.height);
    auto left = (alignment.origin == AlignmentOrigin::top_left ||
                 alignment.origin == AlignmentOrigin::bottom_left)
                    ? horiz
                    : (container.width - horiz - box.width);
    auto top = (alignment.origin == AlignmentOrigin::top_left ||
                alignment.origin == AlignmentOrigin::top_right)
                   ? vert
                   : (container.height - vert - box.height);
    return Position{left, top};
}

Position fractional_align(
    Size container, Size box, FractionalAlignment alignment) {
    return Position{
        (container.width - box.width) * alignment.left,  // left
        (container.height - box.height) * alignment.top  // top
    };
}

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
    auto child_size = adjust_child_size ? fmax(width - align, 0.0f) : width;
    return align + child->query_intrinsic_height(child_size);
}

float AlignedElement::get_intrinsic_width(float height) {
    auto align = alignment.horiz.calc(0);
    auto child_size = adjust_child_size ? fmax(height - align, 0.0f) : height;
    return align + child->query_intrinsic_width(child_size);
}

Size AlignedElement::layout(BoxConstraints constraints) {
    auto child_constraints = constraints;
    if (adjust_child_size) {
        auto horiz = alignment.horiz.calc(constraints.max_width);
        auto vert = alignment.vert.calc(constraints.max_height);
        child_constraints = BoxConstraints{
            0,                                          // min_width
            fmax(constraints.max_width - horiz, 0.0f),  // max_width
            0,                                          // min_height
            fmax(constraints.max_height - vert, 0.0f)   // max_height
        };
    }
    auto size = document->layout_element(child.get(), child_constraints);
    child->size = size;
    child->rel_position =
        positional_align(constraints.max_size(), size, alignment);
    return constraints.max_size();
};

FractionalAlignedElement::FractionalAlignedElement(
    std::shared_ptr<Element> child,
    FractionalAlignment alignment,
    bool is_repaint_boundary)
    : SingleChildElement(
          std::move(child),
          is_repaint_boundary,
          /* size_depends_on_parent */ true),
      alignment(alignment){};

Size FractionalAlignedElement::layout(BoxConstraints constraints) {
    auto size = document->layout_element(child.get(), constraints);
    child->size = size;
    child->rel_position =
        fractional_align(constraints.max_size(), size, alignment);
    return constraints.max_size();
};

}  // namespace aardvark
