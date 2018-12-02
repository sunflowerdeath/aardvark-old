#include "base_types.hpp"

namespace aardvark {

bool Size::is_equal(Size a, Size b) {
return a.width == b.width && a.height == b.height;
};

Position Position::add(Position a, Position b) {
  return Position{
      a.top + b.top,   // left
      a.left + b.left  // top
  };
};

} // namespace aardvark
