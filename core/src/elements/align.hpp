#pragma once

#include <memory>
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
    Align()
        : SingleChildElement(/* child */ nullptr,
                             /* is_repaint_boundary */ false,
                             /* size_depends_on_parent */ true){};

    Align(std::shared_ptr<Element> child, EdgeInsets insets,
          bool adjust_child = true, bool is_repaint_boundary = false);

    std::string get_debug_name() override { return "Align"; };

    Size layout(BoxConstraints constraints) override;
    
    void paint(bool is_changed) override;

    bool hit_test(double left, double top) override;

    // Whether to reduce size of the child by the size of insets, or make it 
    // equal to the size of the container. This is useful when you need to set 
    // relative size and position at the same time.
    bool adjust_child;

    EdgeInsets insets;
};

}  // namespace aardvark::elements
