#include "background.hpp"

namespace aardvark::elements {

Background::Background(SkColor color, bool isRepaintBoundary)
    : Element(isRepaintBoundary), color(color) {};

Size Background::layout(BoxConstraints constraints) {
  return Size{
      constraints.maxWidth /* width */, constraints.maxHeight /* height */
  };
};

void Background::paint() {
  SkPaint paint;
  paint.setColor(color);
  SkRect rect;
  rect.set({absPosition.left, absPosition.top},
           {absPosition.left + size.width, absPosition.top + size.height});
  auto layer = document->getLayer();
  layer->canvas->drawRect(rect, paint);
};

}  // namespace aardvark::elements
