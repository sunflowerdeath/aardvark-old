#pragma once

#include <memory>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"

namespace aardvark::elements {

class GestureResponder : public SingleChildElement {
 public:
  GestureResponder(std::shared_ptr<Element> child,
                   bool is_repaint_boundary = false);

  std::string get_debug_name() override { return "GestureResponder"; };
  Size layout(BoxConstraints constraints) override;
  void paint(bool is_changed) override;
};

} // namespace aardvark::elements
