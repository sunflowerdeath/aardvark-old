#include "border.hpp"
#include <iostream>

namespace aardvark::elements {

Border::Border(std::shared_ptr<Element> child, BoxBorders borders,
               BoxRadiuses radiuses, bool is_repaint_boundary)
    : Element(is_repaint_boundary),
      borders(borders),
      radiuses(radiuses),
      child(child) {
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
      borders.left.width,  // left
      borders.top.width    // top
  };
  return Size{
      size.width + borders.left.width + borders.right.width,  // width
      size.height + borders.top.width + borders.bottom.width  // height
  };
};

void Border::paint() {
  document->paint_element(child.get());

  matrix = SkMatrix();
  layer = document->get_layer();
  layer->canvas->save();
  layer->canvas->translate(abs_position.left, abs_position.top);
  rotation = 0;

  // top
  paint_side(
    borders.left, borders.top, borders.right,
    radiuses.topLeft, radiuses.topRight
  ); 
  // right
  paint_side(
    borders.top, borders.right, borders.bottom,
    radiuses.topRight, radiuses.bottomRight
  );
  // bottom
  paint_side(
    borders.right, borders.bottom, borders.left,
    radiuses.bottomRight, radiuses.bottomLeft
  ); 
  // left
  paint_side(
    borders.bottom, borders.left, borders.top,
    radiuses.bottomLeft, radiuses.topLeft
  );
  layer->canvas->restore();
};

SkPoint calc(SkMatrix& matrix, float left, float top) {
  SkPoint point{left, top};
  matrix.mapPoints(&point, 1);
  return point;
};

void Border::paint_triangle(Radius& radius, BorderSide& prev_side,
                            BorderSide& side, BorderSide& next_side, int width,
                            bool is_left_edge) {
  if (is_left_edge && prev_side.width == 0) return;
  if (!is_left_edge && next_side.width == 0) return;

  SkPath path;
  if (is_left_edge) {
    path.moveTo(calc(matrix, 0, 0));
    path.lineTo(calc(matrix, prev_side.width, 0));
    path.lineTo(calc(matrix, prev_side.width, side.width));
  }
  else {
    path.moveTo(calc(matrix, width, 0));
    path.lineTo(calc(matrix, width - next_side.width, 0));
    path.lineTo(calc(matrix, width - next_side.width, side.width));
  }

  SkPaint paint;
  paint.setStyle(SkPaint::kFill_Style);
  paint.setColor(side.color);
  paint.setAntiAlias(true);
  layer->canvas->drawPath(path, paint);
};

void Border::paint_arc(Radius& radius, BorderSide& side, int width) {
  // TODO change radius width/height to opposite when rotated
  auto line_width = fmin(side.width, radius.width);  // height???
  SkPaint arc_paint;
  arc_paint.setColor(side.color);
  arc_paint.setStyle(SkPaint::kStroke_Style);
  arc_paint.setStrokeWidth(line_width);
  arc_paint.setAntiAlias(true);
  // Adding line_width/2 is needed to paint stroke inside of the oval
  auto lt = calc(matrix, width - 2 * radius.width, line_width / 2);
  auto rb = calc(matrix, width - line_width / 2, 2 * radius.height);
  // When is rotated, coords should be swapped to make correct rect
  float l = lt.x(), t = lt.y(), r = rb.x(), b = rb.y();
  if (l > r) std::swap(l, r);
  if (t > b) std::swap(b, t);
  layer->canvas->drawArc({l, t, r, b},   // oval bounds
                         rotation - 90,  // startAngle, 0 is right middle
                         90,             // sweepAngle
                         false,          // useCenter
                         arc_paint       // paint
  );
};

void Border::paint_side(BorderSide& prev_side, BorderSide& side,
                        BorderSide& next_side, Radius& left_radius,
                        Radius& right_radius) {
  auto width = rotation % 180 == 0 ? size.width : size.height;
  if (side.width == 0) {
    matrix.preTranslate(width, 0);
    matrix.preRotate(90);
    rotation += 90;
    return;
  }

  int start;
  if (left_radius.is_square()) {
    if (prev_side.color == side.color) {
      start = 0;
    } else {
      // Draw triangle on the left side, unless left radius has same color.
      // In that case we can just draw line from beginning of the side
      paint_triangle(left_radius, prev_side, side, next_side, width, true);
      start = prev_side.width;
    }
  } else {
    start = left_radius.width;
  }
  int end =
      width - (right_radius.is_square() ? next_side.width : right_radius.width);

  // Draw the line
  SkPaint line_paint;
  line_paint.setStyle(SkPaint::kStroke_Style);
  line_paint.setColor(side.color);
  line_paint.setStrokeWidth(side.width);
  layer->canvas->drawLine(calc(matrix, start, side.width / 2),
                          calc(matrix, end, side.width / 2), line_paint);

  // Draw different type of transition to next side
  if (right_radius.is_square()) {
    if (side.color != next_side.color) {
      // Draw triangle on the end of the line unless next side has same color
      paint_triangle(right_radius, prev_side, side, next_side, width, false);
    }
  } else {
    // When corner is rounded draw an arc
    paint_arc(right_radius, side, width);
  }
  
  // Transforms applied in reverse order
  matrix.postRotate(90);
  matrix.postTranslate(width, 0);
  rotation += 90;
};

}; // namespace aardvark::elements
