#include "center.hpp"

namespace aardvark::elements {

Center::Center(std::shared_ptr<Element> child, bool is_repaint_boundary)
    : Element(is_repaint_boundary), child(child) {
  child->parent = this;
};

Size Center::layout(BoxConstraints constraints) {
  auto childConstraints = BoxConstraints{
      0,                      // min_width
      constraints.max_width,  // max_width
      0,                      // min_height
      constraints.max_height  // max_height
  };
  auto childSize = document->layout_element(child.get(), childConstraints);
  child->size = childSize;
  child->rel_position = Position{
      (constraints.max_width - childSize.width) / 2,   // left
      (constraints.max_height - childSize.height) / 2  // top
  };
  return Size{
      constraints.max_width,  // width
      constraints.max_height  // height
  };
};

void Center::paint() { document->paint_element(child.get()); };

}  // namespace aardvark::elements
