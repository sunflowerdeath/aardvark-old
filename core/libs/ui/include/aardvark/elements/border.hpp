#pragma once

#include "../base_types.hpp"
#include "../element.hpp"

namespace aardvark {

struct BorderSide {
    float width = 0;
    Color color;

    static BorderSide none() { return BorderSide{0, Color::black()}; };
};

struct BoxBorders {
    BorderSide top;
    BorderSide right;
    BorderSide bottom;
    BorderSide left;

    float height() { return right.width + left.width; }
    float width() { return top.width + bottom.width; }

    static BoxBorders all(BorderSide side) {
        return BoxBorders{side, side, side, side};
    }
};

struct Radius {
    int width = 0;
    int height = 0;

    // Whether the radius is not rounded
    bool is_square() { return width == 0 && height == 0; };

    Radius inner(int d) {
        if (width <= d || height <= d) return Radius{0, 0};
        return Radius{width - d, height - d};
    };

    static Radius circular(int val) { return Radius{val, val}; };
};

struct BoxRadiuses {
    Radius top_left;
    Radius top_right;
    Radius bottom_right;
    Radius bottom_left;

    // Whether all radiuses of the box are square
    bool is_square() {
        return top_left.is_square() && top_right.is_square() &&
               bottom_right.is_square() && bottom_left.is_square();
    };

    static BoxRadiuses all(Radius radius) {
        return BoxRadiuses{radius, radius, radius, radius};
    };
};

class BorderElement : public SingleChildElement {
  public:
    // Two adjacent borders can have rounded corder only when they have same
    // width and color. Connection between borders with rounded corner is
    // painted using an arc.
    // Adjacent borders with square corners may have different widths and
    // colors. In that case connection is painted as triangles.
    BorderElement(
        std::shared_ptr<Element> child,
        BoxBorders borders,
        BoxRadiuses radiuses,
        bool is_repaint_boundary = false);
    BorderElement()
        : SingleChildElement(
              /* child */ nullptr,
              /* is_repaint_boundary */ false,
              /* size_depends_on_parent */ false),
          borders(BoxBorders()),
          radiuses(BoxRadiuses()){};
    std::string get_debug_name() override { return "Border"; };
    float get_intrinsic_height(float width) override;
    float get_intrinsic_width(float height) override;
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;

    ELEMENT_PROP(BoxBorders, borders);
    ELEMENT_PROP(BoxRadiuses, radiuses);

  private:
    SkCanvas* canvas;
    SkPath clip_path;
    SkMatrix clip_matrix;
    SkMatrix matrix;
    int rotation;
    void paint_borders(SkCanvas* canvas);
    void paint_side(
        BorderSide& prev_side,
        BorderSide& side,
        BorderSide& next_side,
        Radius& left_radius,
        Radius& right_radius);
    void clip_side(
        BorderSide& prev_side,
        BorderSide& side,
        BorderSide& next_side,
        Radius& left_radius,
        Radius& right_radius);
    void paint_triangle(
        Radius& radius,
        BorderSide& prev_side,
        BorderSide& side,
        BorderSide& next_side,
        int width,
        bool is_left_edge);
    void paint_arc(Radius& radius, BorderSide& side, int width);
};

}  // namespace aardvark
