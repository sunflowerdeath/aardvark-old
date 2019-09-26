#include "scroll.hpp"

#include <iostream>

namespace aardvark {

float ScrollElement::get_intrinsic_height() {
    return 0;
}

float ScrollElement::get_intrinsic_width() {
    // TODO max child intrinsic width
    return 0;
}

Size ScrollElement::layout(BoxConstraints constraints) {
    auto current_height = 0.0f;
    for (auto& child : children) {
        auto child_constraints = BoxConstraints{
            0,                                      // min_width
            constraints.max_width,                  // max_width
            0,                                      // min_height
            std::numeric_limits<float>::infinity()  // max_width
        };
        auto child_size =
            document->layout_element(child.get(), child_constraints);
        child->size = child_size;
        child->rel_position = Position{0, current_height};
        current_height += child_size.height;
    }
    auto size = constraints.max_size();
    SkPath path;
    path.addRect(0, 0, size.width, size.height);
    clip = path;
    return size;
}

void ScrollElement::paint(bool is_changed) {
    auto total_height = 0.0f;
    for (auto& child : children) total_height += child->size.height;
    document->create_layer({ size.width, total_height });
    for (auto& child : children) document->paint_element(child.get());
    update_transform();
}

void ScrollElement::update_transform() {
    if (layer_tree == nullptr) return;
    auto transform = SkMatrix();
    transform.setTranslate(/* left */ 0, /* top */ -scroll_top);
    layer_tree->transform = transform;
}

void ScrollElement::set_scroll_top(int scroll_top) {
    this->scroll_top = scroll_top;
    update_transform();
    if (document != nullptr) document->need_recompose = true;
}

}  // namespace aardvark
