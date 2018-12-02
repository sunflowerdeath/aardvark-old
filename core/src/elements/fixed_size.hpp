#pragma once

#include <memory>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"

namespace aardvark::elements {

class FixedSize : public Element {
 public:
  FixedSize(std::shared_ptr<Element> child, Size size,
            bool is_repaint_boundary = false);
  Size size;
  std::shared_ptr<Element> child;
  Size layout(BoxConstraints constraints);
  void paint();
};

} // namespace aardvark::elements
