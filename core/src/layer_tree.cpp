#include "layer_tree.hpp"

namespace aardvark {

LayerTree::LayerTree(Element* element) {
  this->element = element; // weak ptr
};

void LayerTree::add(LayerTreeNode item) {
  children.push_back(item);
};

void LayerTree::replace(LayerTreeNode old_item, LayerTreeNode new_item) {
  auto it = std::find(children.begin(), children.end(), old_item);
  children.emplace(it, new_item);
};

void LayerTree::remove_layer(std::shared_ptr<Layer> layer) {
  auto it = std::find(children.begin(), children.end(), LayerTreeNode(layer));
  children.erase(it);
};

std::shared_ptr<Layer> LayerTree::find_by_size(Size size) {
  for (auto item : children) {
    auto layer = std::get_if<std::shared_ptr<Layer>>(&item);
    if (layer == nullptr) return *layer;
  }
  return nullptr;
};

} // namespace aardvark
