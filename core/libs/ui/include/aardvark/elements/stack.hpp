#pragma once

#include "../base_types.hpp"
#include "../element.hpp"

namespace aardvark {

class StackElement : public MultipleChildrenElement {
  public:
    StackElement(
        std::vector<std::shared_ptr<Element>> children = {},
        bool is_repaint_boundary = false)
        : MultipleChildrenElement(
              std::move(children),
              is_repaint_boundary,
              /* size_depends_on_parent */ true){};

    std::string get_debug_name() override { return "Stack"; };
    HitTestMode get_hit_test_mode() override { return HitTestMode::Disabled; };
    Size layout(BoxConstraints constraints) override;
};

}  // namespace aardvark
