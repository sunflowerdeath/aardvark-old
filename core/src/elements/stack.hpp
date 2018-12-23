#pragma once

#include <memory>
#include <vector>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"

namespace aardvark::elements {

class Stack : public MultipleChildrenElement {
 public:
  Stack(std::vector<std::shared_ptr<Element>> children,
        bool is_repaint_boundary = false);

  Size layout(BoxConstraints constraints) override;
  void paint(bool is_changed) override;
};

} // namespace aardvark::elements
