#pragma once

#include <memory>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"

namespace aardvark::elements {

class Center : public Element {
 public:
  Center(std::shared_ptr<Element> child, bool isRepaintBoundary = false);
  std::shared_ptr<Element> child;
	bool sizedByParent = true;
  Size layout(BoxConstraints constraints);
  void paint();
};

} // namespace aardvark::elements
