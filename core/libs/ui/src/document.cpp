#include "document.hpp"

#include "SkPathOps.h"
#include "elements/placeholder.hpp"

namespace aardvark {

SkPath offset_path(SkPath* path, Position offset) {
    SkPath offset_path;
    path->offset(offset.left, offset.top, &offset_path);
    return offset_path;
}

// Add element to set ensuring that no element will be the children of another
void add_only_parent(ElementsSet& set, Element* added) {
    auto it = set.find(added);
    // Already in the set
    if (it != set.end()) return;
    auto children_of_added = std::vector<Element*>();
    for (auto elem : set) {
        // Parent is in the set
        if (elem->is_parent_of(added)) return;
        // Child is in the set
        if (added->is_parent_of(elem)) children_of_added.push_back(elem);
    }
    for (auto elem : children_of_added) set.erase(elem);
    set.insert(added);
};

Document::Document(
    sk_sp<GrContext> gr_context,
    std::shared_ptr<Layer> screen,
    std::shared_ptr<Element> root)
    : gr_context(std::move(gr_context)), screen(std::move(screen)) {
    pointer_event_manager = std::make_unique<PointerEventManager>(this);
    size_observer = std::make_shared<ElementObserver<Size>>(
        [](std::shared_ptr<Element> element) { return element->size; });
    if (root == nullptr) {
        set_root(std::make_shared<PlaceholderElement>());
    } else {
        set_root(root);
    }
}

void Document::set_root(std::shared_ptr<Element> new_root) {
    root = std::move(new_root);
    root->parent = nullptr;
    root->set_document(this);
    root->is_relayout_boundary = true;
    root->is_repaint_boundary = true;
    root->rel_position = Position();
    root->size = screen->size;
    is_initial_render = true;
}

// TODO think if need weak ptrs
void Document::change_element(Element* elem) { changed_elements.insert(elem); }

bool Document::render() {
    if (is_initial_render) {
        return initial_render();
    } else {
        return rerender();
    }
}

bool Document::initial_render() {
    layout_element(
        root.get(), BoxConstraints::from_size(screen->size, true /* tight */));
    size_observer->check_all_elements();
    if (!changed_elements.empty()) relayout();

    current_clip = std::nullopt;
    paint_element(root.get(), /* is_repaint_root */ true);
    compose();
    is_initial_render = false;
    return true;
}

bool Document::rerender() {
    relayout();
    auto painted = repaint();
    if (painted || need_recompose) {
        compose();
        return true;
    } else {
        return false;
    }
}

void Document::relayout() {
    for (auto elem : changed_elements) {
        if (elem->document != this) continue;
        add_only_parent(
            relayout_boundaries, elem->find_closest_relayout_boundary());
    }
    changed_elements.clear();
    for (auto elem : relayout_boundaries) {
        relayout_boundary_element(elem);
    }
    relayout_boundaries.clear();

    size_observer->check_triggered_elements();
    if (!changed_elements.empty()) relayout();
}

void Document::relayout_boundary_element(Element* elem) {
    layout_element(elem, elem->prev_constraints);
    add_only_parent(repaint_boundaries, elem->find_closest_repaint_boundary());
    update_tree_abs_position(elem);
    elem->is_changed = true;
}

void Document::update_tree_abs_position(Element* elem) {
    update_abs_position(elem);
    elem->visit_children([this](std::shared_ptr<Element>& child) {
        update_tree_abs_position(child.get());
    });
}

void Document::update_abs_position(Element* elem) {
    elem->abs_position =
        elem->parent == nullptr
            ? elem->rel_position
            : Position::add(elem->parent->abs_position, elem->rel_position);
}

Size Document::layout_element(Element* elem, BoxConstraints constraints) {
    size_observer->trigger_element(elem->shared_from_this());
    elem->is_relayout_boundary =
        constraints.is_tight() || elem->size_depends_on_parent;
    auto size = elem->layout(constraints);
    elem->prev_constraints = constraints;
    return size;
}

bool Document::repaint() {
    if (repaint_boundaries.empty()) return false;
    for (auto elem : repaint_boundaries) {
        paint_element(elem, /* is_repaint_root */ true);
    }
    repaint_boundaries.clear();
    return true;
}

void Document::partial_relayout(Element* elem) {
    // Check if element is changed or inside changed parent
    auto current = elem;
    auto changed_it = changed_elements.end();
    while (current != nullptr) {
        changed_it = changed_elements.find(current);
        if (changed_it != changed_elements.end()) break;
        current = current->parent;
    }

    if (changed_it != changed_elements.end()) {
        auto boundary = (*changed_it)->find_closest_relayout_boundary();
        relayout_boundary_element(boundary);
        changed_elements.erase(changed_it);
        // TODO should erase from `changed` all elements that are children
        // of the boundary
    }
}

void Document::paint_element(Element* elem, bool is_repaint_root) {
    // std::cout << "paint element: " << elem->get_debug_name() << std::endl;
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

    // Layers from previous layer tree of the current repaint boundary element
    std::vector<LayerTreeNode> prev_layers_pool;
    if (elem->is_repaint_boundary) {
        if (!is_repaint_root && current_layer_tree != nullptr) {
            current_layer_tree->add(elem->layer_tree.get());
        }
        current_layer_tree = elem->layer_tree.get();
        prev_layers_pool = std::move(layers_pool);
        layers_pool = std::move(current_layer_tree->children);
        current_layer = nullptr;
    }

    // Clipping
    auto prev_clip = current_clip;
    if (elem->clip != std::nullopt) {
        // Offset clip to position of the clipped element
        SkPath offset_clip =
            offset_path(&elem->clip.value(), elem->abs_position);
        if (current_clip == std::nullopt) {
            current_clip = offset_clip;
        } else {
            // Intersect prev clip with element's clip and make it new
            // current clip
            Op(current_clip.value(),
               offset_clip,
               kIntersect_SkPathOp,
               &current_clip.value());
        }
    }
    // When element is repaint boundary, current clip is not used while
    // painting, instead it is stored in the layer tree and applied while
    // compositing.
    if (elem->is_repaint_boundary && current_clip != std::nullopt) {
        // Offset clip to the position of the layer
        current_layer_tree->clip = offset_path(
            &current_clip.value(),
            Position{-elem->abs_position.left, -elem->abs_position.top});
        current_clip = std::nullopt;
    }

    auto prev_inside_changed = inside_changed;
    inside_changed = inside_changed || elem->is_changed;
    elem->paint(inside_changed);
    elem->is_changed = false;
    inside_changed = prev_inside_changed;

    current_clip = prev_clip;  // Restore clip
    if (elem->is_repaint_boundary) {
        layers_pool = std::move(prev_layers_pool);
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
        current_clip.value().offset(
            -layer_pos.left, -layer_pos.top, &offset_clip);
        layer->canvas->clipPath(offset_clip, SkClipOp::kIntersect, true);
    }
    layer->canvas->translate(
        elem->abs_position.left - layer_pos.left,
        elem->abs_position.top - layer_pos.top);
}

// Creates layer and adds it to the current layer tree, reusing layers from
// previous repaint if possible.
Layer* Document::create_layer(Size size) {
    auto it = layers_pool.begin();
    while (it != layers_pool.end()) {
        auto prev_layer =
            std::get_if<std::shared_ptr<Layer>>(&*it /* lol ok */);
        if (prev_layer != nullptr &&
            Size::is_equal((*prev_layer)->size, size)) {
            break;
        }
        it++;
    }
    std::shared_ptr<Layer> layer = nullptr;
    if (it == layers_pool.end()) {
        layer = Layer::make_offscreen_layer(gr_context, size);
    } else {
        layer = std::get<std::shared_ptr<Layer>>(*it);
        layer->reset();
        layers_pool.erase(it);
    }
    current_layer_tree->add(layer);
    current_layer = layer.get();
    return current_layer;
}

void Document::compose() {
    need_recompose = false;
    screen->clear();
    current_opacity = 1;
    paint_layer_tree(root->layer_tree.get());
    screen->canvas->flush();
}

void Document::paint_layer_tree(LayerTree* tree) {
    screen->canvas->save();
    auto pos = tree->element->abs_position;
    screen->canvas->translate(pos.left, pos.top);
    screen->canvas->concat(tree->transform);
    if (tree->clip != std::nullopt) {
        // TODO cache/lazy calculate if is expensive
        SkMatrix inverted_transform;
        tree->transform.invert(&inverted_transform);
        SkPath transformed_clip;
        tree->clip.value().transform(inverted_transform, &transformed_clip);
        screen->canvas->clipPath(transformed_clip, SkClipOp::kIntersect, true);
    }
    auto prev_opacity = current_opacity;
    current_opacity *= tree->opacity;
    for (auto item : tree->children) {
        if (std::holds_alternative<LayerTree*>(item)) {
            auto child_tree = std::get<LayerTree*>(item);
            paint_layer_tree(child_tree);
        } else {
            auto child_layer = std::get<std::shared_ptr<Layer>>(item);
            screen->paint_layer(
                child_layer.get(), Position{0, 0}, current_opacity);
        }
    }
    current_opacity = prev_opacity;
    screen->canvas->restore();
}

std::shared_ptr<Connection> Document::add_pointer_event_handler(
    const PointerEventHandler& handler, const bool after_elements) {
    return pointer_event_manager->add_handler(handler, after_elements);
}

std::shared_ptr<Connection> Document::start_tracking_pointer(
    const int pointer_id, const PointerEventHandler& handler) {
    return pointer_event_manager->start_tracking_pointer(pointer_id, handler);
}

std::shared_ptr<Connection> Document::add_key_event_handler(
    const SignalEventSink<KeyEvent>::EventHandler& handler) {
    return key_event_sink.add_handler(handler);
}

std::shared_ptr<Connection> Document::add_scroll_event_handler(
    const SignalEventSink<ScrollEvent>::EventHandler& handler) {
    return scroll_event_sink.add_handler(handler);
}

}  // namespace aardvark
