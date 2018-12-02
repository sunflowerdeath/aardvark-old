#include "background.hpp"

namespace aardvark::elements {

Background::Background(SkColor color, bool is_repaint_boundary)
    : Element(is_repaint_boundary), color(color){};

Size Background::layout(BoxConstraints constraints) {
  return Size{
      constraints.max_width /* width */, constraints.max_height /* height */
  };
};

void Background::paint() {
  SkPaint paint;
  paint.setColor(color);
  SkRect rect;
  rect.set({abs_position.left, abs_position.top},
           {abs_position.left + size.width, abs_position.top + size.height});
  auto layer = document->get_layer();
  layer->canvas->drawRect(rect, paint);
};

}  // namespace aardvark::elements
