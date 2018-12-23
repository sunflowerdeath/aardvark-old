#include "background.hpp"
#include <iostream>

namespace aardvark::elements {

Background::Background(SkColor color, bool is_repaint_boundary)
    : Element(is_repaint_boundary, /* size_depends_on_parent */ true),
      color(color){};

void Background::set_props(SkColor color) {
  this->color = color;
  document->change_element(this);
};

Size Background::layout(BoxConstraints constraints) {
  return Size{
      constraints.max_width /* width */, constraints.max_height /* height */
  };
};

void Background::paint(bool is_changed) {
  SkPaint paint;
  paint.setColor(color);
  auto layer = document->get_layer();
  document->setup_layer(layer, this);
  SkIRect rect{0, 0, size.width, size.height};
  layer->canvas->drawIRect(rect, paint);
};

}  // namespace aardvark::elements
