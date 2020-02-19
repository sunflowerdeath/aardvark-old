#pragma once

#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"

namespace aardvark {

class BackgroundElement : public Element {
  public:
    BackgroundElement()
        : Element(/* is_repaint_boundary */ false,
                  /* size_depends_on_parent */ true){};
    BackgroundElement(Color color, bool is_repaint_boundary = false);
    ELEMENT_PROP_WITH_SETTER(Color, color);
    std::string get_debug_name() override { return "Background"; };
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;
};

}  // namespace aardvark
