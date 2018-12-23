#pragma once

#include <memory>
#include <variant>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"

namespace aardvark::elements {

using AlignmentValue = std::variant<value::rel, value::abs, value::none>;

class Align : public SingleChildElement {
 public:
  Align(std::shared_ptr<Element> child, AlignmentValue left, AlignmentValue top,
        AlignmentValue right = value::none(),
        AlignmentValue bottom = value::none(),
        bool is_repaint_boundary = false);
  AlignmentValue left;
  AlignmentValue top;
  AlignmentValue right;
  AlignmentValue bottom;
  std::string get_debug_name() override { return "Align"; };
  Size layout(BoxConstraints constraints) override;
  void paint(bool is_changed) override;
};

} // namespace aardvark::elements
