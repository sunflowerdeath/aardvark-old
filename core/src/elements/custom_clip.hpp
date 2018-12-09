#pragma once

#include <memory>
#include "SkPath.h"
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"

namespace aardvark::elements {

class CustomClip : public Element {
 public:
  CustomClip(std::shared_ptr<Element> child, SkPath custom_clip_path,
             bool is_repaint_boundary = false);
  SkPath custom_clip_path;
  std::shared_ptr<Element> child;
	bool sized_by_parent = true;
  Size layout(BoxConstraints constraints);
  void paint();
};

} // namespace aardvark::elements
