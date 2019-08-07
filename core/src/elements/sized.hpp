#pragma once

#include <memory>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"

namespace aardvark::elements {

struct SizeConstraints {
    Value min_width = Value::none();
    Value max_width = Value::none();
    Value min_height = Value::none();
    Value max_height = Value::none();

    static SizeConstraints exact(Value width, Value height) {
        return SizeConstraints{width, width, height, height};
    }
};

class Sized : public SingleChildElement {
  public:
    Sized(std::shared_ptr<Element> child, SizeConstraints size,
          bool is_repaint_boundary = false);
    SizeConstraints size_constraints;
    std::string get_debug_name() override { return "Size"; };
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;
};

}  // namespace aardvark::elements
