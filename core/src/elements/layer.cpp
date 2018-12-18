#include "layer.hpp"

namespace aardvark::elements {

Layer::Layer(std::shared_ptr<Element> child,
             std::variant<SkMatrix, TransformOptions> transform)
    : Element(true), child(child) {
  child->parent = this;
  this->transform = transform;
};

Size Layer::layout(BoxConstraints constraints) {
  child->size = document->layout_element(child.get(), constraints.make_loose());
  child->rel_position = Position{0, 0};
  return constraints.max_size();
};

void Layer::paint(bool is_changed) {
  layer_tree->transform = *std::get_if<SkMatrix>(&transform);
  document->paint_element(child.get());
};

}  // namespace aardvark::elements
