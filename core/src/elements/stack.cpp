#include "stack.hpp"

namespace aardvark::elements {

Stack::Stack(std::vector<std::shared_ptr<Element>> children,
             bool is_repaint_boundary)
    : MultipleChildrenElement(children, is_repaint_boundary,
                              /* size_depends_on_parent */ true){};

Size Stack::layout(BoxConstraints constraints) {
    for (auto child : children) {
        child->size =
            document->layout_element(child.get(), constraints.make_loose());
        child->rel_position = Position{0, 0};
    }
    return constraints.max_size();
}

}  // namespace aardvark::elements
