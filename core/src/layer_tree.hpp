#pragma once

#include <memory>
#include <variant>
#include <vector>
#include "element.hpp"
#include "compositing.hpp"

namespace aardvark {

class LayerTree;
class Element;

using LayerTreeNode =
    std::variant<LayerTree*, std::shared_ptr<compositing::Layer>>;

class LayerTree {
 public:
  LayerTree(Element* element);

  // Owner element of the layer tree
  Element* element;

  // Parent layer tree
  LayerTree* parent;

  // Child layers and trees
  std::vector<LayerTreeNode> children;

  // Adds new item to the tree
  void add(LayerTreeNode item);

  // Replace one item with another
  void replace(LayerTreeNode oldItem, LayerTreeNode newItem);

  // Removes item from the tree
  void remove(std::shared_ptr<compositing::Layer> layer);

  // Finds layer in the current tree that has provided size
  std::shared_ptr<compositing::Layer> findBySize(Size size);
};

}
