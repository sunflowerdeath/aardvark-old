#pragma once

#include <memory>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"

namespace aardvark {

struct Alignment {
    Value left = Value::none();
    Value top = Value::none();
    Value right = Value::none();
    Value bottom = Value::none();

    static Alignment all(Value value) {
        return Alignment{value, value, value, value};
    };
};

class AlignElement : public SingleChildElement {
  public:
    AlignElement()
        : SingleChildElement(/* child */ nullptr,
                             /* is_repaint_boundary */ false,
                             /* size_depends_on_parent */ true){};

    AlignElement(
        std::shared_ptr<Element> child,
        Alignment insets,
        bool adjust_child_size = true,
        bool is_repaint_boundary = false);

    std::string get_debug_name() override { return "Align"; };
    float get_intrinsic_height() override;
    float get_intrinsic_width() override;
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;
    HitTestMode get_hit_test_mode() override { return HitTestMode::Disabled; };

    // Whether to reduce size of the child by the size of insets, or make it 
    // equal to the size of the container. This is useful when you need to set 
    // relative size and position at the same time.
    ELEMENT_PROP(bool, adjust_child_size);

    ELEMENT_PROP(Alignment, insets);
};

}  // namespace aardvark::elements
