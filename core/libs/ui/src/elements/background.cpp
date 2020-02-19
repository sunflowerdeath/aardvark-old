#include "elements/background.hpp"

namespace aardvark {

BackgroundElement::BackgroundElement(Color color, bool is_repaint_boundary)
    : Element(is_repaint_boundary, /* size_depends_on_parent */ true),
      color(color){};

Size BackgroundElement::layout(BoxConstraints constraints) {
    return Size{
        constraints.max_width /* width */, constraints.max_height /* height */
    };
};

void BackgroundElement::paint(bool is_changed) {
    SkPaint paint;
    paint.setColor(color.to_sk_color());
    auto layer = document->get_layer();
    document->setup_layer(layer, this);
    SkRect rect{0, 0, size.width, size.height};
    layer->canvas->drawRect(rect, paint);
};

}  // namespace aardvark
