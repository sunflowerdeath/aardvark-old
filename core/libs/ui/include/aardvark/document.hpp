#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <unordered_set>

#include "GrContext.h"
#include "SkCanvas.h"
#include "SkRegion.h"
#include "base_types.hpp"
#include "box_constraints.hpp"
#include "element.hpp"
#include "element_observer.hpp"
#include "layer.hpp"
#include "layer_tree.hpp"
#include "pointer_events/pointer_event_manager.hpp"
#include "pointer_events/signal_event_sink.hpp"

namespace aardvark {

// forward declaration due to circular includes
class Element;
class LayerTree;
class PointerEventManager;

// TODO maybe weak ptr
using ElementsSet = std::unordered_set<Element*>;

using LayerTreeNode = std::variant<LayerTree*, std::shared_ptr<Layer>>;

class Document : public std::enable_shared_from_this<Document> {
  public:
    Document(sk_sp<GrContext> gt_context, std::shared_ptr<Layer> screen,
             std::shared_ptr<Element> root = nullptr);

    // Sets new root element
    void set_root(std::shared_ptr<Element> new_root);

    // Notify document that element has been changed
    void change_element(Element* elem);

    // Notify document that layer properties have beed changed
    void change_layer(Element* elem) { need_recompose = true; };

    // Renders document
    bool render();

    void relayout();

    // Makes the layout of the specified element up-to-date by performing
    // partial relayout of the document.
    void partial_relayout(Element* elem);
    
    void partial_relayout(std::shared_ptr<Element>& elem) {
        partial_relayout(elem.get());
    };

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

    std::shared_ptr<Connection> add_pointer_event_handler(
        const PointerEventHandler& handler, const bool after_elements = false);

    std::shared_ptr<Connection> start_tracking_pointer(
        const int pointer_id, const PointerEventHandler& handler);

    std::shared_ptr<Connection> add_key_event_handler(
        const SignalEventSink<KeyEvent>::EventHandler& handler);

    std::shared_ptr<Connection> add_scroll_event_handler(
        const SignalEventSink<ScrollEvent>::EventHandler& handler);

    std::shared_ptr<Connection> observe_element_size(
        std::shared_ptr<Element> element, std::function<void(Size)> handler) {
        return size_observer->observe(std::move(element), std::move(handler));
    };

    std::shared_ptr<Layer> screen;
    std::shared_ptr<Element> root;
    bool is_initial_render;
    bool need_recompose = false;

    std::unique_ptr<PointerEventManager> pointer_event_manager;
    SignalEventSink<KeyEvent> key_event_sink;
    SignalEventSink<ScrollEvent> scroll_event_sink;

  private:
    bool initial_render();
    bool rerender();
    void relayout_boundary_element(Element* elem);
    bool repaint();
    void compose();
    void paint_layer_tree(LayerTree* tree);

    sk_sp<GrContext> gr_context;
    ElementsSet changed_elements;
    ElementsSet relayout_boundaries;
    ElementsSet repaint_boundaries;
    // Currently painted element
    Element* current_element = nullptr;
    // Layer tree of the current repaint boundary element
    LayerTree* current_layer_tree = nullptr;
    // Layers from previous layer tree of the current repaint boundary element
    std::vector<LayerTreeNode> layers_pool;
    // Layer that is currently used for painting
    Layer* current_layer = nullptr;
    std::optional<SkPath> current_clip = std::nullopt;
    // Whether the current element or some of its parent is changed since last
    // repaint
    bool inside_changed = false;
    float current_opacity = 1;
    std::shared_ptr<ElementObserver<Size>> size_observer;
};

}  // namespace aardvark
