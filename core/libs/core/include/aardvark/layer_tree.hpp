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
    LayerTree* parent = nullptr;

    // Child layers and trees
    std::vector<LayerTreeNode> children;

    std::optional<SkPath> clip;

    SkMatrix transform;

    float opacity = 1;

    // Adds new item to the tree
    void add(LayerTreeNode item);

    // Removes item from the tree
    // void remove(LayerTreeNode layer);

    // Add this tree to new parent, and remove from old one
    // void set_parent(LayerTree* new_parent);
};

}  // namespace aardvark
