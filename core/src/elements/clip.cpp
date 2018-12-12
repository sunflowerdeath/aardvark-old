#include "clip.hpp"

namespace aardvark::elements {

Clip::Clip(std::shared_ptr<Element> child, SkPath (*clipper)(Size),
                       bool is_repaint_boundary)
    : Element(is_repaint_boundary), child(child), clipper(clipper) {
  child->parent = this;
}

Size Clip::layout(BoxConstraints constraints) {
  child->size = document->layout_element(child.get(), constraints.make_loose());
  child->rel_position = Position{0,0};
  return constraints.max_size();
}

void Clip::paint(bool is_changed) {
  document->paint_element(child.get(), false, clipper(size));
}

SkPath Clip::default_clip(Size size) {
  SkPath path;
  path.addRect(0, 0, size.width, size.height);
  return path;
}

}  // namespace aardvark::elements
