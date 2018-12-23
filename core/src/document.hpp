#pragma once

#include <memory>
#include <unordered_set>
#include <optional>
#include "SkCanvas.h"
#include "SkRegion.h"
#include "GrContext.h"
#include "base_types.hpp"
#include "box_constraints.hpp"
#include "layer.hpp"
#include "layer_tree.hpp"
#include "element.hpp"

namespace aardvark {

// forward declaration due to circular includes
class Element;
class LayerTree;

using ElementsSet = std::unordered_set<Element*>;

class Document {
 public:
  Document(std::shared_ptr<Element> root = nullptr);

  sk_sp<GrContext> gr_context;

  // Sets new root element
  void set_root(std::shared_ptr<Element> new_root);

  // Notify document that element was changed
  void change_element(Element* elem);

  // Paints document
  bool paint();

  // Elements should call this function to layout its children
  Size layout_element(Element* elem, BoxConstraints constraints);

  // Elements should call this function to paint its children
  void paint_element(Element* elem, bool is_repaint_root = false);

  // Elements should call this method to obtain layer to paint itself
  Layer* get_layer();

  // Sets translate and clip for painting element on the layer
  void setup_layer(Layer* layer, Element* elem);

  // Creates layer and adds it to the current layer tree, reusing layers from
  // previous repaint if possible.
  Layer* create_layer(Size size);

  void hit_test(double left, double top);
  std::vector<std::shared_ptr<Element>> hit_elements;

  std::shared_ptr<Layer> screen;
  bool is_initial_paint;
 private:
  std::shared_ptr<Element> root;
  ElementsSet changed_elements;

  // These members are used during the paint phase

  // Currently painted element
  Element* current_element = nullptr;
	// Layer tree of the current repaint boundary element
  LayerTree* current_layer_tree = nullptr;
	// Previous layer tree of the current repaint boundary element
  LayerTree* prev_layer_tree = nullptr;
	// Layer that is currently used for painting
  Layer* current_layer = nullptr;
  std::optional<SkPath> current_clip = std::nullopt;
  // Whether the current element or some of its parent is changed since last
  // repaint
  bool inside_changed = false;

  void initial_paint();
  bool repaint();
  void compose_layers();
  void paint_layer_tree(LayerTree* tree);

  void hit_test_element(std::shared_ptr<Element> elem, double left, double top);
  SkMatrix transform;
};

}  // namespace aardvark
