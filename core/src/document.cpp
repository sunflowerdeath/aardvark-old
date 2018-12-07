#include "document.hpp"
#include <iostream>

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
  // std::cout << "REPAINT" << std::endl;
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
  set_clip_region(elem, custom_clip);
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

void Document::set_clip_region(Element* elem,
                               std::optional<SkPath> custom_clip) {
  // TODO optimize when child and parent have same position and side
  SkRegion clip_region;
  if (custom_clip == std::nullopt) {
    // Make default clip region
    clip_region.setRect(elem->abs_position.left,                     // l
                        elem->abs_position.top,                      // t
                        elem->abs_position.left + elem->size.width,  // r
                        elem->abs_position.top + elem->size.height   // b
    );
    std::cout << "default" << elem->abs_position.left << ","
              << elem->abs_position.top << std::endl;
  } else {
    // Make clip region from custom clip path
    clip_region.setPath(custom_clip.value(), SkRegion({0, 0, screen->size.width,
                                                       screen->size.height}));
  }
  if (elem->parent == nullptr) {
    elem->clip_region = clip_region; // explicit move?
  } else {
    // create new clip region from parent
    elem->clip_region = SkRegion(elem->parent->clip_region);
    // intersect elem's own clip with current clip region
    elem->clip_region.op(clip_region, SkRegion::kIntersect_Op);
  }
};

void Document::setup_layer(Layer* layer, Element* elem) {
  layer->canvas->restoreToCount(1);
  layer->canvas->save();
  layer->canvas->translate(elem->abs_position.left, elem->abs_position.top);
  // Clip region is unaffected by transforms
  layer->canvas->clipRegion(elem->clip_region);
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
