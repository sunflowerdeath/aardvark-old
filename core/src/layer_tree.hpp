#pragma once

#include <memory>
#include <variant>
#include <vector>
#include <optional>
#include "SkPath.h"
#include "element.hpp"
#include "compositing.hpp"

namespace aardvark {

class LayerTree;
class Element;

using LayerTreeNode =
    std::variant<LayerTree*, std::shared_ptr<Layer>>;

class LayerTree {
 public:
  LayerTree(Element* element);

  // Owner element of the layer tree
  Element* element;

  // Parent layer tree
  LayerTree* parent;

  // Child layers and trees
  std::vector<LayerTreeNode> children;

  std::optional<SkPath> clip;

  // Adds new item to the tree
  void add(LayerTreeNode item);

  // Replace one item with another
  void replace(LayerTreeNode old_item, LayerTreeNode new_item);

  // Removes layer from the tree
  void remove_layer(std::shared_ptr<Layer> layer);

  // Finds layer in the current tree that has given size
  std::shared_ptr<Layer> find_by_size(Size size);
};

}
