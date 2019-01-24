#include "border.hpp"

namespace aardvark::elements {

Border::Border(std::shared_ptr<Element> child, BoxBorders borders,
               BoxRadiuses radiuses, bool is_repaint_boundary)
    : SingleChildElement(child, is_repaint_boundary,
                         /* size_depends_on_parent */ false),
      borders(borders),
      radiuses(radiuses){};

Size Border::layout(BoxConstraints constraints) {
    auto vert_width = borders.left.width + borders.right.width;
    auto horiz_width = borders.top.width + borders.bottom.width;
    auto child_constraints = BoxConstraints{
        0,                                    // min_width
        constraints.max_width - vert_width,   // max_width
        0,                                    // min_height
        constraints.max_height - horiz_width  // max_height
    };
    auto size = document->layout_element(child.get(), child_constraints);
    child->size = size;
    child->rel_position = Position{
        static_cast<float>(borders.left.width),  // left
        static_cast<float>(borders.top.width)    // top
    };
    return Size{
        size.width + vert_width,   // width
        size.height + horiz_width  // height
    };
};

void Border::paint(bool is_changed) {
    auto layer = document->get_layer();
    document->setup_layer(layer, this);
    this->canvas = layer->canvas;

    // After painting each border side, coordinates are translated and
    // rotated 90 degrees, so next border can be painted with same function.
    matrix = SkMatrix();
    rotation = 0;
    // top -> right -> bottom -> left
    paint_side(borders.left, borders.top, borders.right, radiuses.topLeft,
               radiuses.topRight);
    paint_side(borders.top, borders.right, borders.bottom, radiuses.topRight,
               radiuses.bottomRight);
    paint_side(borders.right, borders.bottom, borders.left,
               radiuses.bottomRight, radiuses.bottomLeft);
    paint_side(borders.bottom, borders.left, borders.top, radiuses.bottomLeft,
               radiuses.topLeft);

    // TODO if all *inner* radiuses are square
    bool need_custom_clip = !radiuses.is_square();
    if (need_custom_clip) {
        clip_matrix = SkMatrix();
        clip_path = SkPath();
        rotation = 0;
        // top -> right -> bottom -> left
        clip_side(borders.left, borders.top, borders.right, radiuses.topLeft,
                  radiuses.topRight);
        clip_side(borders.top, borders.right, borders.bottom, radiuses.topRight,
                  radiuses.bottomRight);
        clip_side(borders.right, borders.bottom, borders.left,
                  radiuses.bottomRight, radiuses.bottomLeft);
        clip_side(borders.bottom, borders.left, borders.top,
                  radiuses.bottomLeft, radiuses.topLeft);
    }
    child->clip = need_custom_clip ? std::optional(clip_path) : std::nullopt;
    document->paint_element(child.get());
};

SkPoint calc(SkMatrix& matrix, float left, float top) {
    SkPoint point{left, top};
    matrix.mapPoints(&point, 1);
    return point;
};

void transform_to_next_side(SkMatrix& matrix, float side_width) {
    // For some unknown reason `preTranslate` is not working same as `preConcat`
    SkMatrix m;
    m.setTranslate(side_width, 0);
    matrix.preConcat(m);
    matrix.preRotate(90);
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
    } else {
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
        transform_to_next_side(matrix, width);
        rotation += 90;
        return;
    }

    float start;
    if (left_radius.is_square()) {
        if (prev_side.color == side.color) {
            // When prev side has the same color, draw line from the beginning
            start = 0;
        } else {
            // When prev side has different color, draw triangle on the left and
            // then draw the line
            paint_triangle(left_radius, prev_side, side, next_side, width,
                           true);
            start = prev_side.width;
        }
    } else {
        start = left_radius.width;
    }
    float end = width - (right_radius.is_square() ? next_side.width
                                                  : right_radius.width);

    // Draw the line
    SkPaint line_paint;
    line_paint.setStyle(SkPaint::kStroke_Style);
    line_paint.setColor(side.color);
    line_paint.setStrokeWidth(side.width);
    line_paint.setAntiAlias(true);
    canvas->drawLine(calc(matrix, start, side.width / 2),
                     calc(matrix, end, side.width / 2), line_paint);

    // Draw different type of transition to next side depending on corner
    if (right_radius.is_square()) {
        if (side.color != next_side.color) {
            // Draw triangle on the end of the line when next side has different
            // color
            paint_triangle(right_radius, prev_side, side, next_side, width,
                           false);
        }
    } else {
        // When corner is rounded draw an arc
        paint_arc(right_radius, side, width);
    }

    transform_to_next_side(matrix, width);
    rotation += 90;
};

void Border::clip_side(BorderSide& prev_side, BorderSide& side,
                       BorderSide& next_side, Radius& left_radius,
                       Radius& right_radius) {
    auto width = (rotation % 180 == 0 ? size.width : size.height) -
                 prev_side.width - next_side.width;
    auto begin = fmax(0, left_radius.width - prev_side.width);
    auto end = width - fmax(0, right_radius.width - next_side.width);
    clip_path.lineTo(calc(clip_matrix, begin, 0));
    clip_path.lineTo(calc(clip_matrix, end, 0));
    auto inner_radius = right_radius.inner(side.width);
    if (!inner_radius.is_square()) {
        auto bounds = SkRect::MakeLTRB(width - 2 * inner_radius.width,  // left
                                       0,                               // top
                                       width,                           // right
                                       2 * inner_radius.height  // height
        );
        clip_matrix.mapRect(&bounds);
        clip_path.arcTo(bounds,         // oval bounds
                        rotation - 90,  // startAngle, 0 is right middle
                        90,             // sweepAngle
                        false           // forceMoveTo
        );
    }

    transform_to_next_side(clip_matrix, width);
    rotation += 90;
};

};  // namespace aardvark::elements
