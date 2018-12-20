#include "fixed_size.hpp"

namespace aardvark::elements {

FixedSize::FixedSize(std::shared_ptr<Element> child, Size size,
                     bool is_repaint_boundary)
    : SingleChildElement(child, is_repaint_boundary), size(size){};

Size FixedSize::layout(BoxConstraints constraints) {
  document->layout_element(child.get(),
                           BoxConstraints::from_size(size, true /* tight */));
  child->size = size;
  child->rel_position = Position{0 /* left */, 0 /* top */};
  return size;
};

void FixedSize::paint(bool is_changed) {
  document->paint_element(child.get());
};

}  // namespace aardvark::elements
