#pragma once

#include <memory>
#include <optional>
#include <variant>
#include <vector>
#include "SkMatrix.h"
#include "SkPath.h"
#include "element.hpp"
#include "layer.hpp"

namespace aardvark {

class LayerTree;
class Element;

using LayerTreeNode = std::variant<LayerTree*, std::shared_ptr<Layer>>;

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

    SkMatrix transform;

    // Adds new item to the tree
    void add(LayerTreeNode item);

    // Replace one item with another
    void replace(LayerTreeNode old_item, LayerTreeNode new_item);

    // Removes layer from the tree
    void remove_layer(std::shared_ptr<Layer> layer);

    // Finds layer in the current tree that has given size
    std::shared_ptr<Layer> find_by_size(Size size);
};

}  // namespace aardvark
