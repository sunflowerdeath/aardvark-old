#include "elements/stack.hpp"

namespace aardvark {

Size StackElement::layout(BoxConstraints constraints) {
    for (auto& child : children) {
        child->size =
            document->layout_element(child.get(), constraints.make_loose());
        child->rel_position = Position{0, 0};
    }
    return constraints.max_size();
}

}  // namespace aardvark
