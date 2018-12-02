#include "layer_tree.hpp"

namespace aardvark {

LayerTree::LayerTree(Element* element) {
  this->element = element; // weak ptr
};

void LayerTree::add(LayerTreeNode item) {
  children.push_back(item);
};

void LayerTree::replace(LayerTreeNode oldItem, LayerTreeNode newItem) {
  auto it = std::find(children.begin(), children.end(), oldItem);
  children.emplace(it, newItem);
};

void LayerTree::remove(std::shared_ptr<compositing::Layer> layer) {
  auto it = std::find(children.begin(), children.end(), LayerTreeNode(layer));
  children.erase(it);
};

std::shared_ptr<compositing::Layer> LayerTree::findBySize(Size size) {
  return nullptr;
};

}
