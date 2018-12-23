#pragma once

#include <memory>
#include "SkPath.h"
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"

namespace aardvark::elements {

class Clip : public SingleChildElement {
 public:
  Clip(std::shared_ptr<Element> child, SkPath (*clipper)(Size),
             bool is_repaint_boundary = false);
  SkPath (*clipper)(Size);
  Size layout(BoxConstraints constraints) override;
  void paint(bool is_changed) override;
  static SkPath default_clip(Size size);

};

} // namespace aardvark::elements
