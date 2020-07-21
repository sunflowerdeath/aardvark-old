#include "base_types.hpp"

namespace aardvark {

bool Size::is_equal(Size a, Size b) {
    return a.width == b.width && a.height == b.height;
};

Position Position::add(Position a, Position b) {
    return Position{
        a.left + b.left,  // left
        a.top + b.top     // top
    };
};

Position Position::origin = Position{0,0};

Color Color::black = Color{0, 0, 0, 255};

}  // namespace aardvark
