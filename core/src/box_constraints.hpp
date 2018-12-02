#pragma once

#include "base_types.hpp"

namespace aardvark {

struct BoxConstraints {
  float min_width;
  float max_width;
  float min_height;
  float max_height;

  // Makes constraints with same max size and zero min size
  BoxConstraints make_loose();

  // Checks whether min constraints is same as max constraints
  bool is_tight();

  // Makes tight or loose constraints from size
  static BoxConstraints from_size(Size size, bool tight);
};

} // namespace aardvark
