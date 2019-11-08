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

class Document {
  public:
    Document(std::shared_ptr<Element> root = nullptr);

    // Sets new root element
    void set_root(std::shared_ptr<Element> new_root);

    // Notify document that element was changed
    void change_element(Element* elem);

    // Renders document
    bool render();

    void relayout();

    // Makes the layout of the specified element up-to-date by performing
    // partial relayout of the document.
    void partial_relayout(Element* elem);

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

    ElementObserverConnection<Size> observe_element_size(
        std::shared_ptr<Element>, std::function<void(Size)> handler);

    std::shared_ptr<Layer> screen;
    std::shared_ptr<Element> root;
    bool is_initial_render;
    std::unique_ptr<PointerEventManager> pointer_event_manager;
    SignalEventSink<KeyEvent> key_event_sink;
    SignalEventSink<ScrollEvent> scroll_event_sink;
    bool need_recompose = false;

  private:
    bool initial_render();
    bool rerender();
    void relayout_boundary_element(Element* elem);
    bool repaint();
    void compose();
    void paint_layer_tree(LayerTree* tree);

    sk_sp<GrContext> gr_context;
    std::shared_ptr<ElementObserver<Size>> size_observer;
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
};

}  // namespace aardvark
