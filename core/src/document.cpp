#include "document.hpp"
#include <iostream>
#include "SkPathOps.h"
#include "elements/placeholder.hpp"

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

Document::Document(std::shared_ptr<Element> root) {
    gr_context = GrContext::MakeGL();
    screen = Layer::make_screen_layer(gr_context);
    pointer_event_manager = std::make_unique<PointerEventManager>(this);
    if (root == nullptr) {
        set_root(std::make_shared<elements::Placeholder>());
    } else {
        set_root(root);
    }
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
    layout_element(root.get(),
                   BoxConstraints::from_size(screen->size, true /* tight */));
    current_clip = std::nullopt;
    paint_element(root.get(), /* is_repaint_root */ true);
    compose_layers();
    is_initial_paint = false;
}

bool Document::repaint() {
    if (changed_elements.empty()) {
        compose_layers();
        return false;  // nothing to repaint
    }
    // TODO check when compose is needed
    ElementsSet relayout_boundaries;
    for (auto elem : changed_elements) {
        add_only_parent(relayout_boundaries,
                        elem->find_closest_relayout_boundary());
    }
    ElementsSet repaint_boundaries;
    for (auto elem : relayout_boundaries) {
        layout_element(elem, elem->prev_constraints);
        add_only_parent(repaint_boundaries,
                        elem->find_closest_repaint_boundary());
        elem->is_changed = true;
    }
    for (auto elem : repaint_boundaries) {
        paint_element(elem, /* is_repaint_root */ true);
    }
    changed_elements.clear();
    compose_layers();
    return true;
}

Size Document::layout_element(Element* elem, BoxConstraints constraints) {
    elem->document = this;
    elem->is_relayout_boundary =
        constraints.is_tight() || elem->size_depends_on_parent;
    auto size = elem->layout(constraints);
    elem->prev_constraints = constraints;
    return size;
}

SkPath offset_path(SkPath* path, Position offset) {
    SkPath offset_path;
    path->offset(offset.left, offset.top, &offset_path);
    return offset_path;
}

void Document::paint_element(Element* elem, bool is_repaint_root) {
    // std::cout << "paint element: " << elem->get_debug_name() << std::endl;
    if (!is_repaint_root) elem->parent = current_element;
    current_element = elem;

    /* 
    TODO
    if (elem->controls_layer_tree) {
        elem->paint(); // Sets elem's `layer_tree` property
        // External element never changes, only its parent can cause a repaint,
        // so `current_layer_tree` is always set here.
        current_layer_tree->add(elem->layer_tree);
        return;
    }
    */

    auto pin_prev_layer_tree = elem->layer_tree;  // pin shared pointer
    if (elem->is_repaint_boundary) {
        // Save previous tree to be able to reuse layers from it
        prev_layer_tree = elem->layer_tree.get();
        LayerTree* parent_tree =
            is_repaint_root
                ? (is_initial_paint ? nullptr : prev_layer_tree->parent)
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

    // Clipping
    auto prev_clip = current_clip;
    if (is_repaint_root) {
        // Repaint root doesn't get clip, because its paint is called by 
        // the document, not by the parent element, so it restores its clip
        // from the prev layer tree.
        if (!is_initial_paint) current_layer_tree->clip = prev_layer_tree->clip;
    } else {
        if (elem->clip != std::nullopt) {
            // Offset clip to position of the clipped element
            SkPath offset_clip =
                offset_path(&elem->clip.value(), elem->abs_position);
            if (current_clip == std::nullopt) {
                current_clip = offset_clip;
            } else {
                // Intersect prev clip with element's clip and make it new
                // current clip
                Op(current_clip.value(), offset_clip, kIntersect_SkPathOp,
                   &current_clip.value());
            }
        }
        // When element is repaint boundary, current clip is not needed while
        // painting, instead it is applied while compositing.
        if (elem->is_repaint_boundary && current_clip != std::nullopt) {
            // Offset clip to the position of the layer
            current_layer_tree->clip = offset_path(
                &current_clip.value(),
                Position{-elem->abs_position.left, -elem->abs_position.top});
            current_clip = std::nullopt;
        }
    }

    auto prev_inside_changed = inside_changed;
    inside_changed = inside_changed || elem->is_changed;
    elem->paint(inside_changed);
    elem->is_changed = false;
    inside_changed = prev_inside_changed;

    current_clip = prev_clip;  // Restore clip
    if (elem->is_repaint_boundary) {
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

void Document::setup_layer(Layer* layer, Element* elem) {
    layer->canvas->restoreToCount(1);
    layer->canvas->save();
    auto layer_pos = current_layer_tree->element->abs_position;
    if (current_clip != std::nullopt) {
        SkPath offset_clip;
        current_clip.value().offset(-layer_pos.left, -layer_pos.top,
                                    &offset_clip);
        layer->canvas->clipPath(offset_clip, SkClipOp::kIntersect, true);
    }
    layer->canvas->translate(elem->abs_position.left - layer_pos.left,
                             elem->abs_position.top - layer_pos.top);
}

// Creates layer and adds it to the current layer tree, reusing layers from
// previous repaint if possible.
Layer* Document::create_layer(Size size) {
    std::shared_ptr<Layer> layer = prev_layer_tree != nullptr
                                       ? prev_layer_tree->find_by_size(size)
                                       : nullptr;
    if (layer == nullptr) {
        // Create new layer
        layer = Layer::make_offscreen_layer(gr_context, size);
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
    screen->canvas->save();
    auto pos = tree->element->abs_position;
    screen->canvas->translate(pos.left, pos.top);
    screen->canvas->concat(tree->transform);
    if (tree->clip != std::nullopt) {
        screen->canvas->clipPath(tree->clip.value(), SkClipOp::kIntersect,
                                 true);
    }
    for (auto item : tree->children) {
        if (std::holds_alternative<LayerTree*>(item)) {
            auto child_tree = std::get<LayerTree*>(item);
            paint_layer_tree(child_tree);
        } else {
            auto child_layer = std::get<std::shared_ptr<Layer>>(item);
            screen->paint_layer(child_layer.get(), Position{0, 0});
        }
    }
    screen->canvas->restore();
}

/*
void Document::handle_event(PointerEvent event) {
    hit_tester->test(root, event.left, event.top);
    reconciler->reconcile(event, hit_tester->hit_elements, root.get());
};
*/

};  // namespace aardvark
