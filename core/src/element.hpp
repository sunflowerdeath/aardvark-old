#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include "SkPath.h"
#include "base_types.hpp"
#include "box_constraints.hpp"
#include "document.hpp"
#include "pointer_events/responder.hpp"
#include "pointer_events/hit_tester.hpp"

namespace aardvark {

// Forward declarations due to circular includes
class Document;
class LayerTree;
class HitTester;

enum class HitTestMode {
    // After element handles event, it passes it to the element that is behind.
    PassThrough,

    // Passes event to the parent element (or any further ancestor) behind this
    // element.
    // This is default mode.
    PassToParent,

    // Does not pass event after handling.
    Absorb,

    // Disabled
    Disabled
};

using ChildrenVisitor = std::function<void(std::shared_ptr<Element>)>;

// Base class for elements of the document
class Element : public std::enable_shared_from_this<Element> {
    friend Document;
    friend HitTester;

  public:
    Element(bool is_repaint_boundary, bool size_depends_on_parent);

    virtual ~Element(){};

    // -------------------------------------------------------------------------
    // These fields can be set with default constructor:
    // -------------------------------------------------------------------------

    // Repaint boundary element does not share layers with another elements.
    // This allows to repaint this element separately.
    bool is_repaint_boundary;

    // Should be `true` when its size depends only on input constraints, not on
    // element's props or children. This allows to optimize relayout.
    bool size_depends_on_parent;

    // -------------------------------------------------------------------------
    // These methods can be overriden to implement different elements:
    // -------------------------------------------------------------------------

    // Returns name of the element for debugging
    virtual std::string get_debug_name() { return "Element"; };

    // In this method element should calculate its size, layout children
    // and set their size and relative positions.
    virtual Size layout(BoxConstraints constraints) {
        return constraints.max_size();
    };

    // Returns minimum height that element could fit into.
    virtual float get_intrinsic_height() { return 0; }

    // Returns minimum width that element could fit into.
    virtual float get_intrinsic_width() { return 0; }

    // Paints element and its children.
    // `is_changed` is `true` when the element itself or some of its parents is
    // changed. When it is `false`, element is allowed to reuse result of
    // previous painting.
    virtual void paint(bool is_changed){};

    // Walks children in paint order.
    virtual void visit_children(ChildrenVisitor visitor){};

    // Checks if element is hit by pointer. Default is checking element's box.
    virtual bool hit_test(double left, double top);

    // Default is `PassToParent`.
    virtual HitTestMode get_hit_test_mode();

    // Element must have only one responder, and it must ensure that returned
    // pointer is valid during all of its lifetime.
    virtual Responder* get_responder() { return nullptr; };

    // These methods only needed for elements with children
    virtual void append_child(std::shared_ptr<Element> child){};
    virtual void remove_child(std::shared_ptr<Element> child){};
    virtual void insert_before_child(std::shared_ptr<Element> child,
                                     std::shared_ptr<Element> before_child){};

    // -------------------------------------------------------------------------
    // These props should be set by the parent element during layout
    // -------------------------------------------------------------------------
    Size size;
    Position rel_position;
    std::optional<SkPath> clip = std::nullopt;

    // Notifies the document, that this element was changed
    void change();

    // Checks whether the element is direct or indirect parent of another
    // element
    bool is_parent_of(Element* elem);

    void set_document(Document* new_document);

    Element* find_closest_relayout_boundary();
    Element* find_closest_repaint_boundary();

    // Parent element should set this during constructing and updating
    Element* parent = nullptr;

    // Document is set when this element is painted
    Document* document = nullptr;

    // Absolute position is calculated before painting the element
    Position abs_position;

    bool controls_layer_tree = false;

    std::shared_ptr<LayerTree> layer_tree;

  private:
    // Whether the element was changed by updating props or performig relayout
    // since last repaint.
    bool is_changed;

    // When element is relayout boundary, changes inside it do not affect
    // layout of parents. This happens when element recieves tight constraints,
    // so it is always same size, or when element's size depends only on
    // input constraints.
    bool is_relayout_boundary = false;

    // This is used for relayout
    BoxConstraints prev_constraints;
};

class SingleChildElement : public Element {
  public:
    SingleChildElement(std::shared_ptr<Element> child, bool is_repaint_boundary,
                       bool size_depends_on_parent);

    float get_intrinsic_height() override {
        return child->get_intrinsic_height();
    }
    float get_intrinsic_width() override {
        return child->get_intrinsic_width();
    }
    void paint(bool is_changed) override;
    void append_child(std::shared_ptr<Element> child) override;
    void remove_child(std::shared_ptr<Element> child) override;
    void visit_children(ChildrenVisitor visitor) override { visitor(child); };

    std::shared_ptr<Element> child;
};

class MultipleChildrenElement : public Element {
  public:
    MultipleChildrenElement(std::vector<std::shared_ptr<Element>> children,
                            bool is_repaint_boundary,
                            bool size_depends_on_parent);

    float get_intrinsic_height() override;
    float get_intrinsic_width() override;
    void paint(bool is_changed) override;
    void remove_child(std::shared_ptr<Element> child) override;
    void append_child(std::shared_ptr<Element> child) override;
    void insert_before_child(std::shared_ptr<Element> child,
                             std::shared_ptr<Element> before_child) override;
    void visit_children(ChildrenVisitor visitor) override;

    std::vector<std::shared_ptr<Element>> children;
};

}  // namespace aardvark
