#pragma once

#include <memory>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"

namespace aardvark::elements {

class Align : public Element {
 public:
  Align(std::shared_ptr<Element> child, int left, int top,
        bool is_repaint_boundary = false);
  std::shared_ptr<Element> child;
  int left;
  int top;
	bool sized_by_parent = true;
  Size layout(BoxConstraints constraints);
  void paint();
};

} // namespace aardvark::elements
