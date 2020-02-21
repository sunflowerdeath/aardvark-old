#include "elements/layer.hpp"

namespace aardvark {

Size LayerElement::layout(BoxConstraints constraints) {
    child->size =
        document->layout_element(child.get(), constraints.make_loose());
    child->rel_position = Position{0, 0};
    return child->size;
    // return constraints.max_size();
}

}  // namespace aardvark
