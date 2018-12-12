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
  cache = PaintCache();
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

void Border::paint(bool is_changed) {
  document->paint_element(child.get());

  auto layer = document->get_layer();
  document->setup_layer(layer, this);
  paint_borders(layer->canvas);
  // cache.restart(is_changed);
  // cache.paint(layer->canvas, [&](SkCanvas* canvas) { paint_borders(canvas); });
}

void Border::paint_borders(SkCanvas* canvas) {
  this->canvas = canvas;
  matrix = SkMatrix();
  // layer->canvas->save();
  // layer->canvas->translate(abs_position.left, abs_position.top);
  rotation = 0;

  // After painting each border side, coordinates are translated and
  // rotated 90 degrees, so next border can be painted with same function.

  /*
  bool need_custom_clip = !radiuses.is_square();
  if (need_custom_clip) {
    SkMatrix clip_matrix;
    SkPath clip_path;
  }
  */

  // top
  paint_side(
    borders.left, borders.top, borders.right,
    radiuses.topLeft, radiuses.topRight
  );
  /*
  need_custom_clip && clip_side(
    borders.left, borders.top, borders.right,
    radiuses.topLeft, radiuses.topRight
  );
  */
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
  canvas->drawPath(path, paint);
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
  auto bounds = SkRect::MakeLTRB(width - 2 * radius.width,  // l
                                 line_width / 2,            // t
                                 width - line_width / 2,    // r
                                 2 * radius.height          // b
  );
  matrix.mapRect(&bounds);
  canvas->drawArc(bounds,         // oval
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
  canvas->drawLine(calc(matrix, start, side.width / 2),
                          calc(matrix, end, side.width / 2), line_paint);

  // Draw different type of transition to next side depending on corner
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

void Border::clip_side(BorderSide& prev_side, BorderSide& side,
                       BorderSide& next_side, Radius& left_radius,
                       Radius& right_radius) {
  auto width = rotation % 180 == 0 ? size.width : size.height;
  auto begin = fmax(prev_side.width, left_radius.width);
  auto end = width - fmax(next_side.width, right_radius.width);
  clip_path.lineTo(calc(clip_matrix, begin, side.width));
  clip_path.lineTo(calc(clip_matrix, end, side.width));
  auto inner_radius = right_radius.inner(side.width);
  if (inner_radius.width > 0 && inner_radius.height > 0) {
    /*
    clip_path->drawArc({l, t, r, b},   // oval bounds
                       rotation - 90,  // startAngle, 0 is right middle
                       90,             // sweepAngle
                       false,          // useCenter
                       false           // forceMoveTo
    );
    */
  }
  clip_matrix.postRotate(90);
  clip_matrix.postTranslate(width, 0);
};

}; // namespace aardvark::elements
