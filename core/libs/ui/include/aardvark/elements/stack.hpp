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
              /* size_depends_on_parent */ false){};

    std::string get_debug_name() override { return "Stack"; };
    HitTestMode get_hit_test_mode() override { return HitTestMode::Disabled; };
    Size layout(BoxConstraints constraints) override;
    
    // Whether to loosen layout constraints for childs
    ELEMENT_PROP_DEFAULT(bool, loosen_constraints, true);
};

class StackChildElement : public SingleChildElement {
  public:
    StackChildElement(
        std::shared_ptr<Element> child = {},
        bool floating = false,
        bool is_repaint_boundary = false)
        : floating(floating),
          SingleChildElement(
              std::move(child),
              is_repaint_boundary,
              /* size_depends_on_parent */ false){};

    std::string get_debug_name() override { return "StackChild"; };
    HitTestMode get_hit_test_mode() override { return HitTestMode::Disabled; };
    
    // Floating children do not affect size of the stack
    ELEMENT_PROP_DEFAULT(bool, floating, true);
};

}  // namespace aardvark
