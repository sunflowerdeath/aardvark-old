#pragma once

#include <memory>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"

namespace aardvark {

enum class AlignmentOrigin { top_left, top_right, bottom_left, bottom_right };

struct Alignment {
    AlignmentOrigin origin = AlignmentOrigin::top_left;
    Value vert = Value::none();
    Value horiz = Value::none();

    static Alignment top_left(Value top, Value left) {
        return Alignment{AlignmentOrigin::top_left, top, left};
    }
    static Alignment top_right(Value top, Value right) {
        return Alignment{AlignmentOrigin::top_right, top, right};
    }
    static Alignment bottom_left(Value bottom, Value left) {
        return Alignment{AlignmentOrigin::bottom_left, bottom, left};
    }
    static Alignment bottom_right(Value bottom, Value right) {
        return Alignment{AlignmentOrigin::bottom_right, bottom, right};
    }
};

class AlignedElement : public SingleChildElement {
  public:
    AlignedElement()
        : SingleChildElement(/* child */ nullptr,
                             /* is_repaint_boundary */ false,
                             /* size_depends_on_parent */ true){};

    AlignedElement(
        std::shared_ptr<Element> child,
        Alignment alignment,
        bool adjust_child_size = true,
        bool is_repaint_boundary = false);

    std::string get_debug_name() override { return "Align"; };
    float get_intrinsic_height(float width) override;
    float get_intrinsic_width(float height) override;
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;
    HitTestMode get_hit_test_mode() override { return HitTestMode::Disabled; };

    // Whether to reduce size of the child by the size of insets, or make it 
    // equal to the size of the container. This is useful when you need to set 
    // relative size and position at the same time.
    ELEMENT_PROP(bool, adjust_child_size);

    ELEMENT_PROP(Alignment, alignment);
};

}  // namespace aardvark::elements
