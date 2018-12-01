#pragma once

#include <memory>
#include <unordered_set>

#include "base_types.hpp"
#include "box_constraints.hpp"
#include "compositing.hpp"
#include "element.hpp"
#include "layer_tree.hpp"

namespace aardvark {

// forward declaration due to circular includes
class Element;
class LayerTree;

using ElementsSet = std::unordered_set<Element*>;

class Document {
 public:
  Document(compositing::Compositor& compositor, std::shared_ptr<Element> root);
  void setRoot(std::shared_ptr<Element> newRoot);
  void changeElement(Element* elem);
  void paint();

 private:
  compositing::Compositor compositor;
  std::shared_ptr<compositing::Layer> screen;
  std::shared_ptr<Element> root;
  ElementsSet changedElements;
  bool isInitialPaint;

  // These members are used during the paint phase
  Element* currentElement;
  LayerTree* currentLayerTree;
  LayerTree* prevLayerTree;
  compositing::Layer* currentLayer;

  void initialPaint();
  void repaint();
  Size layoutElement(Element* elem, BoxConstraints constraints);
  bool currentClip;
  void paintElement(Element* elem, bool isRepaintRoot, bool clip);

  compositing::Layer* getLayer();

  // Creates layer and adds it to the current layer tree, reusing layers from
  // previous repaint if possible.
  compositing::Layer* createLayer(Size size);

  void composeLayers();

  void paintLayerTree(LayerTree* tree);
};

}  // namespace aardvark
