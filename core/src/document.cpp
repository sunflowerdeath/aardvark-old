#include "document.hpp"
#include <iostream>
#include "SkPathOps.h"

namespace aardvark {

// Add element to set ensuring that no element will be the children of another
void add_only_parent(ElementsSet& set, Element* added) {
  for (auto elem : set) {
    if (elem->is_parent_of(added)) {
      return;  // Parent is already in the set
    }
    if (added->is_parent_of(elem)) {
      set.erase(elem);  // Child is in the set
    }
  }
  set.insert(added);
};

Document::Document(Compositor& compositor, std::shared_ptr<Element> root)
    : compositor(compositor), screen(compositor.make_screen_layer()) {
  set_root(root);
}

void Document::set_root(std::shared_ptr<Element> new_root) {
  root = new_root;
  root->is_repaint_boundary = true;
  root->rel_position = Position();
  root->size = screen->size;
  is_initial_paint = true;
}

void Document::change_element(Element* elem) { changed_elements.insert(elem); }

bool Document::paint() {
  if (is_initial_paint) {
    initial_paint();
    return true;
  } else {
    return repaint();
  }
}

void Document::initial_paint() {
  // std::cout << "INITIAL PAINT" << std::endl;
  layout_element(root.get(),
                 BoxConstraints::from_size(screen->size, true /* tight */));
  paint_element(root.get(), /* isRepaintRoot */ true);
  compose_layers();
  is_initial_paint = false;
}

bool Document::repaint() {
  if (changed_elements.empty()) return false;  // nothing to repaint
  ElementsSet relayout_boundaries;
  for (auto elem : changed_elements) {
    add_only_parent(relayout_boundaries,
                    elem->find_closest_relayout_boundary());
  }
  ElementsSet repaint_boundaries;
  for (auto elem : relayout_boundaries) {
    layout_element(elem, elem->prev_constraints);
    add_only_parent(repaint_boundaries, elem->find_closest_repaint_boundary());
  }
  for (auto elem : repaint_boundaries) {
    paint_element(elem, /* isRepaintRoot */ true);
  }
  compose_layers();
  changed_elements.clear();
  return true;
}

Size Document::layout_element(Element* elem, BoxConstraints constraints) {
  // std::cout << "layout element: " << elem->get_debug_name() << std::endl;
  elem->document = this;
  elem->is_relayout_boundary = constraints.is_tight() || !elem->sized_by_parent;
  auto size = elem->layout(constraints);
  elem->prev_constraints = constraints;
  return size;
}

void Document::paint_element(Element* elem, bool is_repaint_root,
                             std::optional<SkPath> custom_clip) {
  // std::cout << "paint element: " << elem->get_debug_name() << std::endl;
  if (!is_repaint_root) elem->parent = current_element;
  this->current_element = elem;
  // TODO handle this stuff
  auto prev_layer_tree_sp = elem->layer_tree;
  if (elem->is_repaint_boundary) {
    // Save previous tree to be able to reuse layers from it
    prev_layer_tree = elem->layer_tree.get();
    LayerTree* parent_tree =
        is_repaint_root ? (is_initial_paint ? nullptr : prev_layer_tree->parent)
                        : current_layer_tree;
    // Create new tree and add it to the parent tree
    elem->layer_tree = std::make_shared<LayerTree>(elem);
    elem->layer_tree->parent = parent_tree;
    if (parent_tree != nullptr) {
      if (is_repaint_root) {
        parent_tree->replace(prev_layer_tree, elem->layer_tree.get());
      } else {
        parent_tree->add(elem->layer_tree.get());
      }
    }
    // Make new tree current
    current_layer_tree = elem->layer_tree.get();
    current_layer = nullptr;
  }
  elem->abs_position =
      elem->parent == nullptr
          ? elem->rel_position
          : Position::add(elem->parent->abs_position, elem->rel_position);
  set_clip_path(elem, custom_clip);
  elem->paint();
  if (elem->is_repaint_boundary) {
    // Reset current layer tree when leaving repaint boundary
    prev_layer_tree = nullptr;
    current_layer_tree = current_layer_tree->parent;
    current_layer = nullptr;
  }
  current_element = elem->parent;
}

Layer* Document::get_layer() {
  // If there is no current layer, setup default layer
  Layer* layer;
  if (current_layer == nullptr) {
    layer = create_layer(current_layer_tree->element->size);
  } else {
    layer = current_layer;
  }
  layer->set_changed();
  return layer;
}

SkPath get_elem_clip(Element* elem, std::optional<SkPath> custom_clip) {
  if (custom_clip != std::nullopt) {
    SkPath clip_path;
    custom_clip.value().offset(elem->abs_position.left, elem->abs_position.top,
                               &clip_path);
    return clip_path;
  }
  SkPath clip_path;
  clip_path.addRect(elem->abs_position.left,                     // l
                    elem->abs_position.top,                      // t
                    elem->abs_position.left + elem->size.width,  // r
                    elem->abs_position.top + elem->size.height   // b
  );
  return clip_path;
}

void Document::set_clip_path(Element* elem, std::optional<SkPath> custom_clip) {
  if (elem->parent == nullptr) {
    elem->clip_path = get_elem_clip(elem, custom_clip);
  } else {
    if (custom_clip == std::nullopt && elem->size == elem->parent->size &&
        elem->abs_position == elem->parent->abs_position) {
      // When element uses default clip and has same size and position as its
      // parent, just reuse parent's clip
      elem->clip_path = elem->parent->clip_path;
    } else {
      // Intersect elem's own clip with current clip region
      Op(elem->parent->clip_path, get_elem_clip(elem, custom_clip),
         kIntersect_SkPathOp, &elem->clip_path);
    }
  }
};

void Document::setup_layer(Layer* layer, Element* elem) {
  layer->canvas->restoreToCount(1);
  layer->canvas->save();
  layer->canvas->clipPath(elem->clip_path, SkClipOp::kIntersect, true);
  layer->canvas->translate(elem->abs_position.left, elem->abs_position.top);
};

// Creates layer and adds it to the current layer tree, reusing layers from
// previous repaint if possible.
Layer* Document::create_layer(Size size) {
  std::shared_ptr<Layer> layer = prev_layer_tree != nullptr
                                     ? prev_layer_tree->find_by_size(size)
                                     : nullptr;
  if (layer == nullptr) {
    // Create new layer
    layer = compositor.make_offscreen_layer(size);
  } else {
    // Reuse layer
    prev_layer_tree->remove_layer(layer);
    layer->reset();
  }
  current_layer_tree->add(layer);
  current_layer = layer.get();
  return layer.get();
}

void Document::compose_layers() {
  screen->clear();
  paint_layer_tree(root->layer_tree.get());
  screen->canvas->flush();
}

void Document::paint_layer_tree(LayerTree* tree) {
  for (auto item : tree->children) {
    auto child_tree = std::get_if<LayerTree*>(&item);
    if (child_tree != nullptr) {
      paint_layer_tree(*child_tree);
    } else {
      auto child_layer = *std::get_if<std::shared_ptr<Layer>>(&item);
      compositor.paint_layer(screen.get(), child_layer.get(),
                             tree->element->abs_position);
    }
  }
}

};  // namespace aardvark
