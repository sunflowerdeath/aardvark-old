#pragma once

#include "../base_types.hpp"
#include "../element.hpp"

namespace aardvark {

class StackElement : public MultipleChildrenElement {
  public:
    StackElement(
        std::vector<std::shared_ptr<Element>> children = {},
        bool loosen_constraints = true,
        bool is_repaint_boundary = false)
        : loosen_constraints(loosen_constraints),
          MultipleChildrenElement(
              std::move(children),
              is_repaint_boundary,
              /* size_depends_on_parent */ true){};

    std::string get_debug_name() override { return "Stack"; };
    HitTestMode get_hit_test_mode() override { return HitTestMode::Disabled; };
    Size layout(BoxConstraints constraints) override;
    
    // Whether to loosen layout constraints for childs
    ELEMENT_PROP_DEFAULT(bool, loosen_constraints, true);
};

}  // namespace aardvark
