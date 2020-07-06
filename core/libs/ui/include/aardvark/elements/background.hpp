#pragma once

#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"

namespace aardvark {

class BackgroundElement : public SingleChildElement {
  public:
    BackgroundElement()
        : SingleChildElement(
              nullptr,
              /* is_repaint_boundary */ false,
              /* size_depends_on_parent */ false){};

    BackgroundElement(
        std::shared_ptr<Element> child,
        Color color,
        bool after = false,
        bool is_repaint_boundary = false)
        : SingleChildElement(
              std::move(child),
              is_repaint_boundary,
              /* size_depends_on_parent */ true),
          after(after),
          color(color){};

    std::string get_debug_name() override { return "Background"; };
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;

    ELEMENT_PROP(Color, color);  // TODO transparent
    ELEMENT_PROP_DEFAULT(bool, after, false);

  private:
    void paint_background();
};

}  // namespace aardvark
