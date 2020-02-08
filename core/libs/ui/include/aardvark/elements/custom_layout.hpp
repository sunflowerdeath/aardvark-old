#pragma once

#include <functional>
#include <memory>

#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"

namespace aardvark::elements {

using CustomLayoutProc = std::function<Size(Element*, BoxConstraints)>;

class CustomLayout : public MultipleChildrenElement {
  public:
    CustomLayout()
        : MultipleChildrenElement({}, /* is_repaint_boundary */ false,
                                  /* size_depends_on_parent */ false){};

    CustomLayout(std::vector<std::shared_ptr<Element>> children,
                 CustomLayoutProc layout_func, bool is_repaint_boundary)
        : layout_proc(layout_proc),
          MultipleChildrenElement(children, is_repaint_boundary,
                                  /* size_depends_on_parent */ false){};

    std::string get_debug_name() override { return "CustomLayout"; };

    Size layout(BoxConstraints constraints) override {
        return layout_proc(this, constraints);
    };

    CustomLayoutProc layout_proc;
};

}  // namespace aardvark::elements
