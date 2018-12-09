#include "custom_clip.hpp"

namespace aardvark::elements {

CustomClip::CustomClip(std::shared_ptr<Element> child, SkPath custom_clip_path,
                       bool is_repaint_boundary)
    : Element(is_repaint_boundary),
      child(child),
      custom_clip_path(custom_clip_path) {
  child->parent = this;
};

Size CustomClip::layout(BoxConstraints constraints) {
  child->size = document->layout_element(child.get(), constraints.make_loose());
  child->rel_position = Position{0,0};
  return constraints.max_size();
};

void CustomClip::paint() {
  document->paint_element(child.get(), false, custom_clip_path);
};

}  // namespace aardvark::elements
