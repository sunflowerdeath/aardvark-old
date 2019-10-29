#include "layer.hpp"

namespace aardvark::elements {

Size Layer::layout(BoxConstraints constraints) {
    child->size =
        document->layout_element(child.get(), constraints.make_loose());
    child->rel_position = Position{0, 0};
    return child->size;
    // return constraints.max_size();
};

void Layer::paint(bool is_changed) {
    layer_tree->transform = transform;
    document->paint_element(child.get());
};

}  // namespace aardvark::elements
