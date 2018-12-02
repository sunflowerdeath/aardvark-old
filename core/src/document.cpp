#include "document.hpp"
#include <iostream>

namespace aardvark {

void addOnlyParent(ElementsSet &set, Element* added) {
  for (auto elem : set) {
    if (elem->isParentOf(added)) {
      return; // Parent is already in the set
    }
    if (added->isParentOf(elem)) {
      set.erase(elem); // Child is in the set
    }
  }
  set.insert(added);
};

Document::Document(compositing::Compositor& compositor,
                   std::shared_ptr<Element> root)
    : compositor(compositor), screen(compositor.get_screen_layer()) {
  setRoot(root);
}

void Document::setRoot(std::shared_ptr<Element> newRoot) {
  root = newRoot;
  root->isRepaintBoundary = true;
  root->relPosition = Position();
  root->size = screen->size;
  isInitialPaint = true;
}

void Document::changeElement(Element* elem) {
  changedElements.insert(elem);
}

void Document::paint() {
  if (isInitialPaint) {
    initialPaint();
  } else {
    repaint();
  }
}

void Document::initialPaint() {
  layoutElement(root.get(), BoxConstraints::fromSize(screen->size));
  paintElement(root.get(), /* isRepaintRoot */ true, /* clip */ false);
  composeLayers();
  isInitialPaint = false;
}

void Document::repaint() {
  if (changedElements.empty()) return;  // nothing to repaint
  ElementsSet relayoutBoundaries;
  for (auto elem : changedElements) {
    addOnlyParent(relayoutBoundaries, elem->findClosestRelayoutBoundary());
  }
  ElementsSet repaintBoundaries;
  for (auto elem : relayoutBoundaries) {
    layoutElement(elem, elem->prevConstraints);
    addOnlyParent(repaintBoundaries, elem->findClosestRepaintBoundary());
  }
  for (auto elem : repaintBoundaries) {
    paintElement(elem, /* isRepaintRoot */ true, /* clip */ false);
  }
  composeLayers();
  changedElements.clear();
}

Size Document::layoutElement(Element* elem, BoxConstraints constraints) {
  elem->document = this;
  elem->isRelayoutBoundary = constraints.isTight() || !elem->sizedByParent;
  auto size = elem->layout(constraints);
  elem->prevConstraints = constraints;
  return size;
}

void Document::paintElement(Element* elem, bool isRepaintRoot, bool clip) {
  currentClip = clip;
  if (!isRepaintRoot) elem->parent = currentElement;
  this->currentElement = elem;
  if (elem->isRepaintBoundary) {
    // Save previous tree to be able to reuse layers from it
    prevLayerTree = elem->layerTree.get();
    // Create new tree and add it to the parent tree
    LayerTree* parentTree =
        isRepaintRoot
            ? (isInitialPaint ? nullptr : prevLayerTree->parent)
            : currentLayerTree;
    elem->layerTree = std::make_shared<LayerTree>(elem);
    if (parentTree != nullptr) {
      elem->layerTree->parent = parentTree;
      if (isRepaintRoot) {
        parentTree->replace(prevLayerTree, elem->layerTree.get());
      } else {
        parentTree->add(elem->layerTree.get());
      }
    }
    // Make new tree current
    currentLayerTree = elem->layerTree.get();
    currentLayer = nullptr;
  }
  elem->absPosition =
      elem->parent == nullptr
          ? elem->relPosition
          : Position::add(elem->parent->absPosition, elem->relPosition);
  elem->paint();
  if (elem->isRepaintBoundary) {
    // Reset current layer tree when leaving repaint boundary
    prevLayerTree = nullptr;
    currentLayerTree = currentLayerTree->parent;
    currentLayer = nullptr;
  }
  currentElement = elem->parent;
}

compositing::Layer* Document::getLayer() {
  // If there is no current layer, setup default layer
  if (!currentLayer) createLayer(currentLayerTree->element->size);
  return currentLayer;
  // TODO setup translate and clip
}

// Creates layer and adds it to the current layer tree, reusing layers from
// previous repaint if possible.
compositing::Layer* Document::createLayer(Size size) {
  std::shared_ptr<compositing::Layer> layer =
      prevLayerTree != nullptr ? prevLayerTree->findBySize(size) : nullptr;
  if (layer == nullptr) {
    // Create new layer
    layer = compositor.create_offscreen_layer(size);
  } else {
    // Reuse layer
    prevLayerTree->remove(layer);
    layer.reset();
  }
  currentLayerTree->add(layer);
  currentLayer = layer.get();
  std::cout << "create layer ok" << std::endl;
  return layer.get();
}

void Document::composeLayers() {
  screen->clear();
  paintLayerTree(root->layerTree.get());
  screen->canvas->flush();
}

void Document::paintLayerTree(LayerTree* tree) {
  for (auto &item : tree->children) {
    auto child_tree = std::get_if<LayerTree*>(&item);
    if (child_tree != nullptr) {
      paintLayerTree(*child_tree);
    } else {
      auto child_layer =
         *std::get_if<std::shared_ptr<compositing::Layer>>(&item);
      compositor.paint_layer(screen.get(), child_layer.get(),
                             tree->element->absPosition);
    }
  }
}

};  // namespace aardvark
