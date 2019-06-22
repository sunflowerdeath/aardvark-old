#pragma once

#include <memory>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"

namespace aardvark::elements {

class Size : public SingleChildElement {
  public:
    Size(std::shared_ptr<Element> child, Value min_width, Value max_width,
         Value min_height, Value max_height, bool is_repaint_boundary = false);
    Value max_width;
    Value max_height;
    Value min_width;
    Value min_height;
    std::string get_debug_name() override { return "Size"; };
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;
};

}  // namespace aardvark::elements
