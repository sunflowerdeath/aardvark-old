#pragma once

#include <memory>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"
#include "../paint_cache.hpp"
#include "SkCanvas.h"

namespace aardvark::elements {

struct BorderSide {
    float width = 0;
    SkColor color;

    static BorderSide none() { return BorderSide{0, SK_ColorBlack}; };
};

struct BoxBorders {
    BorderSide top;
    BorderSide right;
    BorderSide bottom;
    BorderSide left;

    static BoxBorders all(BorderSide side) {
        return BoxBorders{side, side, side, side};
    };
};

struct Radius {
    int width;
    int height;

    // Whether the radius is not rounded
    bool is_square() { return width == 0 && height == 0; };

    Radius inner(int d) {
        if (width <= d || height <= d) return Radius{0, 0};
        return Radius{width - d, height - d};
    };

    static Radius circular(int val) { return Radius{val, val}; };
};

struct BoxRadiuses {
    Radius topLeft;
    Radius topRight;
    Radius bottomRight;
    Radius bottomLeft;

    // Whether all radiuses of the box are square
    bool is_square() {
        return topLeft.is_square() && topRight.is_square() &&
               bottomRight.is_square() && bottomLeft.is_square();
    };

    static BoxRadiuses all(Radius radius) {
        return BoxRadiuses{radius, radius, radius, radius};
    };
};

class Border : public SingleChildElement {
  public:
    // Two adjacent borders can have rounded corder only when they have same
    // width and color. Connection between borders with rounded corner is
    // painted using an arc.
    // Adjacent borders with square corners may have different widths and
    // colors. In that case connection is painted as triangles.
    Border(std::shared_ptr<Element> child, BoxBorders borders,
           BoxRadiuses radiuses, bool is_repaint_boundary = false);
    BoxBorders borders;
    BoxRadiuses radiuses;
    std::string get_debug_name() override { return "Border"; };
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;

  private:
    SkCanvas* canvas;
    SkPath clip_path;
    SkMatrix clip_matrix;
    SkMatrix matrix;
    int rotation;
    void paint_borders(SkCanvas* canvas);
    void paint_side(BorderSide& prev_side, BorderSide& side,
                    BorderSide& next_side, Radius& left_radius,
                    Radius& right_radius);
    void clip_side(BorderSide& prev_side, BorderSide& side,
                   BorderSide& next_side, Radius& left_radius,
                   Radius& right_radius);
    void paint_triangle(Radius& radius, BorderSide& prev_side, BorderSide& side,
                        BorderSide& next_side, int width, bool is_left_edge);
    void paint_arc(Radius& radius, BorderSide& side, int width);
};

}  // namespace aardvark::elements
