#pragma once

#include <memory>
#include <variant>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"

namespace aardvark::elements {

using AlignmentValue = std::variant<value::rel, value::abs, value::none>;

class Align : public Element {
 public:
  Align(std::shared_ptr<Element> child, AlignmentValue left, AlignmentValue top,
        AlignmentValue right = value::none(),
        AlignmentValue bottom = value::none(),
        bool is_repaint_boundary = false);
  std::shared_ptr<Element> child;
  AlignmentValue left;
  AlignmentValue top;
  AlignmentValue right;
  AlignmentValue bottom;
  bool sized_by_parent = true;
  Size layout(BoxConstraints constraints);
  void paint();
};

} // namespace aardvark::elements
