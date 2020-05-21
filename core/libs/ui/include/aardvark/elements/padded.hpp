#pragma once

#include "../base_types.hpp"
#include "../element.hpp"

namespace aardvark {

struct Insets {
    float left = 0.0;
    float top = 0.0;
    float right = 0.0;
    float bottom = 0.0;

    float width() { return left + right; }
    float height() { return top + bottom; }
};

class PaddedElement : public SingleChildElement {
  public:
    PaddedElement()
        : SingleChildElement(
              /* child */ nullptr,
              /* is_repaint_boundary */ false,
              /* size_depends_on_parent */ false){};

    PaddedElement(
        std::shared_ptr<Element> child,
        Insets padding,
        bool is_repaint_boundary = false)
        : SingleChildElement(
              std::move(child),
              is_repaint_boundary,
              /* size_depends_on_parent */ false),
          padding(padding){};

    std::string get_debug_name() override { return "Padding"; };
    float get_intrinsic_height(float width) override;
    float get_intrinsic_width(float height) override;
    Size layout(BoxConstraints constraints) override;

    ELEMENT_PROP(Insets, padding);
};

}  // namespace aardvark
