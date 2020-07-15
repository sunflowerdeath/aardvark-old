#include "elements/stack.hpp"

namespace aardvark {

Size StackElement::layout(BoxConstraints constraints) {
    auto max_width = 0.0f;
    auto max_height = 0.0f;
    auto floating_children = std::vector<Element*>();
    for (auto& child : children) {
        auto stack_child = dynamic_cast<StackChildElement*>(child.get());
        if (stack_child != nullptr && stack_child->floating) {
            floating_children.push_back(child.get());
            continue;
        }
        child->size = document->layout_element(
            child.get(),
            loosen_constraints ? constraints.make_loose() : constraints);
        child->rel_position = Position{0, 0};
        max_width = fmax(max_width, child->size.width);
        max_height = fmax(max_height, child->size.height);
    }
    auto floating_constraints = BoxConstraints{
        loosen_constraints ? 0 : constraints.min_width,   // min_width
        max_width,                                        // max_width
        loosen_constraints ? 0 : constraints.min_height,  // min_height
        max_height                                        // max_height
    };
    for (auto child : floating_children) {
        child->size = document->layout_element(child, floating_constraints);
        child->rel_position = Position{0, 0};
    }
    return Size{max_width, max_height};
}

}  // namespace aardvark
