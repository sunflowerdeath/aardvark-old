#pragma once

#include <memory>
#include <variant>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"

namespace aardvark::elements {

struct EdgeInsets {
    Value left = Value::none();
    Value top = Value::none();
    Value right = Value::none();
    Value bottom = Value::none();

    static EdgeInsets all(Value value) {
        return EdgeInsets{value, value, value, value};
    };
};

class Align : public SingleChildElement {
  public:
    Align(std::shared_ptr<Element> child, EdgeInsets insets,
          bool is_repaint_boundary = false);
    std::string get_debug_name() override { return "Align"; };
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;
    EdgeInsets insets;
};

}  // namespace aardvark::elements
