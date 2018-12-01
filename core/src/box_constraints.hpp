#pragma once

#include "base_types.hpp"

namespace aardvark {

struct BoxConstraints {
  float minWidth;
  float maxWidth;
  float minHeight;
  float maxHeight;

  BoxConstraints makeLoose();

  bool isTight();

  static BoxConstraints fromSize(Size size);
};

} // namespace aardvark
