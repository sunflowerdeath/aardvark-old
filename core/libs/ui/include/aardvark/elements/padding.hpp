#pragma once

#include "../base_types.hpp"
#include "../element.hpp"

namespace aardvark {

struct Padding {
    float left = 0.0;
    float top = 0.0;
    float right = 0.0;
    float bottom = 0.0;
};

class PaddingElement : public SingleChildElement {
  public:
    PaddingElement()
        : SingleChildElement(
              /* child */ nullptr,
              /* is_repaint_boundary */ false,
              /* size_depends_on_parent */ false){};

    PaddingElement(
        std::shared_ptr<Element> child,
        Padding padding,
        bool is_repaint_boundary = false)
        : SingleChildElement(
              std::move(child),
              is_repaint_boundary,
              /* size_depends_on_parent */ false),
          padding(padding){};

    std::string get_debug_name() override { return "Padding"; };
    float get_intrinsic_height() override;
    float get_intrinsic_width() override;
    Size layout(BoxConstraints constraints) override;

    ELEMENT_PROP_WITH_SETTER(Padding, padding);
};

}  // namespace aardvark
