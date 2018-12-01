#include "box_constraints.hpp"

namespace aardvark {

BoxConstraints BoxConstraints::makeLoose() {
  return BoxConstraints{
      0,         // minWidth
      maxWidth,  // maxWidth
      0,         // minHeight
      maxHeight  // maxHeight
  };
};

bool BoxConstraints::isTight() {
  return minWidth == maxWidth && minHeight == maxHeight;
};

BoxConstraints BoxConstraints::fromSize(Size size) {
  return BoxConstraints{
      size.width,   // minWidth
      size.width,   // maxWidth
      size.height,  // minHeight
      size.height   // maxHeight
  };
};

}  // namespace aardvark
