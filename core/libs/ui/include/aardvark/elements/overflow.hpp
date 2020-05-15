#pragma once

#include "../element.hpp"

namespace aardvark {

class OverflowElement : public SingleChildElement {
  public:
    OverflowElement()
        : SingleChildElement(
              /* child */ nullptr,
              /* is_repaint_boundary */ false,
              /* size_depends_on_parent */ false){};

    OverflowElement(
        std::shared_ptr<Element> child,
        std::optional<float> max_width,
        std::optional<float> max_height)
        : max_width(max_width),
          max_height(max_height),
          SingleChildElement(
              /* child */ std::move(child),
              /* is_repaint_boundary */ false,
              /* size_depends_on_parent */ false){};

    std::string get_debug_name() override { return "Overflow"; };
    Size layout(BoxConstraints constraints) override;

    ELEMENT_PROP_DEFAULT(std::optional<float>, max_width, std::nullopt);
    ELEMENT_PROP_DEFAULT(std::optional<float>, max_height, std::nullopt);
};

}  // namespace aardvark
