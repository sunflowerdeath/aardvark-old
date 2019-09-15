#include "center.hpp"

namespace aardvark::elements {

Size Center::layout(BoxConstraints constraints) {
    auto child_size =
        document->layout_element(child.get(), constraints.make_loose());
    child->size = child_size;
    child->rel_position = Position{
        (constraints.max_width - child_size.width) / 2,   // left
        (constraints.max_height - child_size.height) / 2  // top
    };
    return constraints.max_size();
};

}  // namespace aardvark::elements
