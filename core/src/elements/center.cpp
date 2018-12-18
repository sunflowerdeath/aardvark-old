#include "center.hpp"

namespace aardvark::elements {

Center::Center(std::shared_ptr<Element> child, bool is_repaint_boundary)
    : Element(is_repaint_boundary), child(child) {
  child->parent = this;
};

Size Center::layout(BoxConstraints constraints) {
  auto child_size =
      document->layout_element(child.get(), constraints.make_loose());
  child->size = child_size;
  child->rel_position = Position{
      (constraints.max_width - child_size.width) / 2,   // left
      (constraints.max_height - child_size.height) / 2  // top
  };
  return constraints.max_size();
};

void Center::paint(bool is_changed) { document->paint_element(child.get()); };

}  // namespace aardvark::elements
