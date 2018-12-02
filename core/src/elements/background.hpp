#pragma once

#include <memory>
#include "SkCanvas.h"
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"

namespace aardvark::elements {

class Background : public Element {
 public:
  Background(SkColor color, bool isRepaintBoundary = false);
  SkColor color;
  std::shared_ptr<Element> child;
  Size layout(BoxConstraints constraints) override;
  void paint() override;
};

} // namespace aardvark::elements
