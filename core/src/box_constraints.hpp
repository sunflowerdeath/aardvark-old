#pragma once

#include "base_types.hpp"

namespace aardvark {

struct BoxConstraints {
  int min_width;
  int max_width;
  int min_height;
  int max_height;

  // Makes constraints with same max size and zero min size
  BoxConstraints make_loose();

  // Checks whether min constraints is same as max constraints
  bool is_tight();

  // Returns max possible size
  Size max_size();

  // Makes tight or loose constraints from size
  static BoxConstraints from_size(Size size, bool tight);
};

} // namespace aardvark
