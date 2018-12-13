#include <iostream>
#include "layer_tree.hpp"

namespace aardvark {

LayerTree::LayerTree(Element* element) {
  this->element = element;
};

void LayerTree::add(LayerTreeNode item) {
  children.push_back(item);
};

void LayerTree::replace(LayerTreeNode old_item, LayerTreeNode new_item) {
  std::replace(children.begin(), children.end(), old_item, new_item);
};

void LayerTree::remove_layer(std::shared_ptr<Layer> layer) {
  auto it = std::find(children.begin(), children.end(), LayerTreeNode(layer));
  children.erase(it);
};

std::shared_ptr<Layer> LayerTree::find_by_size(Size size) {
  for (auto item : children) {
    auto tree = std::get_if<LayerTree*>(&item);
    auto layer = std::get_if<std::shared_ptr<Layer>>(&item);
    if (layer != nullptr && Size::is_equal((*layer)->size, size)) {
      return *layer;
    }
  }
  return nullptr;
};

} // namespace aardvark
