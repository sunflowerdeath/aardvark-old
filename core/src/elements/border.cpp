#include "border.hpp"

namespace aardvark::elements {

Border::Border(std::shared_ptr<Element> child, BoxBorders borders,
               bool is_repaint_boundary)
    : Element(is_repaint_boundary), borders(borders), child(child) {
  child->parent = this;
};

Size Border::layout(BoxConstraints constraints) {
  auto child_constraints = BoxConstraints{
      0,  // min_width
      constraints.max_width - borders.left.width -
          borders.right.width,  // max_width
      0,                        // min_height
      constraints.max_height - borders.top.width -
          borders.bottom.width  // max_height
  };
  auto size = document->layout_element(child.get(), child_constraints);
  child->size = size;
  child->rel_position = Position{
      borders.top.width,  // top
      borders.left.width  // left
  };
  return Size{
      size.width + borders.left.width + borders.right.width,  // width
      size.height + borders.top.width + borders.bottom.width  // height
  };
};

void Border::paint() {
  document->paint_element(child.get());
  auto layer = document->get_layer();
  SkPaint paint;
  auto border = borders.top;
  paint.setStyle(SkPaint::kStroke_Style);
  paint.setStrokeWidth(border.width);
  paint.setColor(border.color);
  // paint.setAntiAlias(true);
  // paint.setStrokeJoin(SkPaint::kRound_Join);
  SkRect rect;
  rect.setXYWH(abs_position.left + border.width / 2,  // left
               abs_position.top + border.width / 2,   // top
               size.width - border.width,             // width
               size.height - border.width             // height
  );
  layer->canvas->drawRect(rect, paint);
};

}; // namespace aardvark::elements
