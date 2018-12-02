#pragma once

#include <memory>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"

namespace aardvark::elements {

class Center : public Element {
 public:
  Center(std::shared_ptr<Element> child, bool is_repaint_boundary = false);
  std::shared_ptr<Element> child;
	bool sized_by_parent = true;
  Size layout(BoxConstraints constraints);
  void paint();
};

} // namespace aardvark::elements
