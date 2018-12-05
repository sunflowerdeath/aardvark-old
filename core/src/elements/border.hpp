#pragma once

#include <memory>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"
#include "SkCanvas.h"

namespace aardvark::elements {

struct BorderSide {
  int width = 0;
  SkColor color;
};

struct BoxBorders {
  BorderSide top;
  BorderSide right;
  BorderSide bottom;
  BorderSide left;
};

class Border : public Element {
 public:
  Border(std::shared_ptr<Element> child, BoxBorders borders,
         bool is_repaint_boundary = false);
  BoxBorders borders;
  std::shared_ptr<Element> child;
  Size layout(BoxConstraints constraints);
  void paint();
};

} // namespace aardvark::elements
