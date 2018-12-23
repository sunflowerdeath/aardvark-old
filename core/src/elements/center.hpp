#pragma once

#include <memory>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"

namespace aardvark::elements {

class Center : public SingleChildElement {
  public:
    Center(std::shared_ptr<Element> child, bool is_repaint_boundary = false);
    std::string get_debug_name() override { return "Center"; };
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;
};

}  // namespace aardvark::elements
