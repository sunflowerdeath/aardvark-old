#include "align.hpp"

namespace aardvark::elements {

Align::Align(std::shared_ptr<Element> child, int left, int top,
             bool is_repaint_boundary)
    : Element(is_repaint_boundary), left(left), top(top), child(child) {
  child->parent = this;
};

Size Align::layout(BoxConstraints constraints) {
  int abs_left = abs(left);
  int abs_top = abs(top);
  auto child_constraints = BoxConstraints{
      0,                                 // min_width
      constraints.max_width - abs_left,  // max_width
      0,                                 // min_height
      constraints.max_height - abs_top   // max_height
  };
  auto size = document->layout_element(child.get(), child_constraints);
  child->size = size;
  child->rel_position = Position{
      left < 0 ? (size.width - abs_left) : abs_left,  // left
      top < 0 ? (size.height - abs_top) : abs_top     // top
  };
  return constraints.max_size();
};

void Align::paint() { document->paint_element(child.get()); };

}  // namespace aardvark::elements
