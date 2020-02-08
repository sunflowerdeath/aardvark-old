#pragma once

#include "../element.hpp"

namespace aardvark {

class ScrollElement : public MultipleChildrenElement {
  public:
    ScrollElement()
        : MultipleChildrenElement(/* children */ {},
                                  /* is_repaint_boundary */ true,
                                  /* size_depends_on_parent */ true){};

    ScrollElement(std::vector<std::shared_ptr<Element>> children)
        : MultipleChildrenElement(children,
                                  /* is_repaint_boundary */ true,
                                  /* size_depends_on_parent */ true){};

    std::string get_debug_name() override { return "Scroll"; };
    float get_intrinsic_height() override;
    float get_intrinsic_width() override;
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;

    void set_scroll_top(int scroll_top);
    int scroll_top = 0;
    int scroll_left = 0;
    float scroll_height = 0.0f;

  private:
    void update_transform();
};

}  // namespace aardvark
