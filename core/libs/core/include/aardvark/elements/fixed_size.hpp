#pragma once

#include <memory>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"

namespace aardvark::elements {

class FixedSize : public SingleChildElement {
  public:
    FixedSize(std::shared_ptr<Element> child, Size size,
              bool is_repaint_boundary = false);
    Size size;
    std::string get_debug_name() override { return "FixedSize"; };
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;
};

}  // namespace aardvark::elements
