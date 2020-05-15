#pragma once

#include <limits>
#include "../element.hpp"

namespace aardvark {

enum class OverflowType { original, unconstrained, sized };

struct OverflowConstraint {
    OverflowType type;
    float size;

    float resolve(float original);

    static OverflowConstraint original;
    static OverflowConstraint unconstrained;
    static OverflowConstraint sized(float size);
};

class OverflowElement : public SingleChildElement {
  public:
    OverflowElement()
        : SingleChildElement(
              /* child */ nullptr,
              /* is_repaint_boundary */ false,
              /* size_depends_on_parent */ false){};

    OverflowElement(
        std::shared_ptr<Element> child,
        OverflowConstraint max_width,
        OverflowConstraint max_height)
        : max_width(max_width),
          max_height(max_height),
          SingleChildElement(
              /* child */ std::move(child),
              /* is_repaint_boundary */ false,
              /* size_depends_on_parent */ false){};

    std::string get_debug_name() override { return "Overflow"; };
    Size layout(BoxConstraints constraints) override;

    ELEMENT_PROP_DEFAULT(
        OverflowConstraint, max_width, OverflowConstraint::original);
    ELEMENT_PROP_DEFAULT(
        OverflowConstraint, max_height, OverflowConstraint::original);
};

}  // namespace aardvark
