#pragma once

#include <functional>

#include "../base_types.hpp"
#include "../element.hpp"

namespace aardvark {

using CustomLayoutFn =
    std::function<Size(std::shared_ptr<Element>, BoxConstraints)>;

class CustomLayoutElement : public MultipleChildrenElement {
  public:
    CustomLayoutElement()
        : MultipleChildrenElement(
              {},
              /* is_repaint_boundary */ false,
              /* size_depends_on_parent */ false){};

    CustomLayoutElement(
        std::vector<std::shared_ptr<Element>> children,
        CustomLayoutFn layout_fn,
        bool is_repaint_boundary)
        : layout_fn(std::move(layout_fn)),
          MultipleChildrenElement(
              std::move(children),
              is_repaint_boundary,
              /* size_depends_on_parent */ false){};

    std::string get_debug_name() override { return "CustomLayout"; };

    Size layout(BoxConstraints constraints) override {
        return layout_fn(shared_from_this(), constraints);
    };

    ELEMENT_PROP(CustomLayoutFn, layout_fn);
};

}  // namespace aardvark
