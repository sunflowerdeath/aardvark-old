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

  void add(LayerTreeNode item);
  void replace(LayerTreeNode oldItem, LayerTreeNode newItem);
  void remove(LayerTreeNode item);
  std::shared_ptr<compositing::Layer> findBySize(Size size);
};

}
