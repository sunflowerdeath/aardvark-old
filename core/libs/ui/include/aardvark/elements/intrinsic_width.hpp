#pragma once

#include "../element.hpp"

namespace aardvark {

class IntrinsicWidthElement : public SingleChildElement {
  public:
    IntrinsicWidthElement()
        : SingleChildElement(
              /* child */ nullptr,
              /* is_repaint_boundary */ false,
              /* size_depends_on_parent */ false){};

    IntrinsicWidthElement(
        std::shared_ptr<Element> child, bool is_repaint_boundary = false)
        : SingleChildElement(
              std::move(child),
              is_repaint_boundary,
              /* size_depends_on_parent */ false){};

    std::string get_debug_name() override { return "IntrinsicWidth"; };

    Size layout(BoxConstraints constraints) override {
        auto size = Size{child->get_intrinsic_width(constraints.max_width),
                         constraints.max_height};
        auto child_constraints =
            BoxConstraints::from_size(size, /* tight */ false);
        auto child_size =
            document->layout_element(child.get(), child_constraints);
        child->size = child_size;
        child->rel_position = Position{0, 0};
        return child_size;
    };
};

}  // namespace aardvark
