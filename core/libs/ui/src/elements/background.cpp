#include "elements/background.hpp"

namespace aardvark {

Size BackgroundElement::layout(BoxConstraints constraints) {
    if (child == nullptr) {
        return Size{
            constraints.max_width /* width */,
            constraints.max_height /* height */
        };
    }
    auto child_size = child->layout(constraints);
    child->size = child_size;
    child->rel_position = Position{0, 0};
    return child_size;
};

void BackgroundElement::paint_background() {
    auto layer = document->get_layer();
    document->setup_layer(layer, this);
    SkPaint paint;
    paint.setColor(color.to_sk_color());
    SkRect rect{0, 0, size.width, size.height};
    layer->canvas->drawRect(rect, paint);
}

void BackgroundElement::paint(bool is_changed) {
    if (!after) paint_background();
    if (child != nullptr) document->paint_element(child.get());
    if (after) paint_background();
};

}  // namespace aardvark
