#include "layer_tree.hpp"

namespace aardvark {

LayerTree::LayerTree(Element* element) {
  this->element = element; // weak ptr
};

void add(LayerTreeNode item) {};
void replace(LayerTreeNode oldItem, LayerTreeNode newItem) {};
void remove(std::shared_ptr<compositing::Layer> layer) {};
std::shared_ptr<compositing::Layer> findBySize(Size size) {
  return nullptr;
};

}
