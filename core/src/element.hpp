#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include "SkPath.h"
#include "base_types.hpp"
#include "box_constraints.hpp"
#include "responder.hpp"
#include "document.hpp"

namespace aardvark {

// Forward declarations due to circular includes
class Document;
class LayerTree;
class Responder;
// In order to forward declare enum, it must tell compiler how much memory
// it uses
enum class ResponderMode : unsigned int;

using ChildrenVisitor = std::function<void(std::shared_ptr<Element>)>;

// Base class for elements of the document
class Element {
    friend Document;

  public:
    Element(bool is_repaint_boundary, bool size_depends_on_parent);

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
    virtual std::string get_debug_name() { return "Unknown element"; };

    // In this method element should calculate its size, layout children
    // and set their size and relative positions.
    virtual Size layout(BoxConstraints constraints){};

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
    virtual ResponderMode get_responder_mode();
    virtual Responder* get_responder() { return nullptr; };

    // These methods only needed for elements with children
    virtual void append_child(std::shared_ptr<Element> child){};
    virtual void remove_child(std::shared_ptr<Element> child){};
    virtual void insert_before_child(std::shared_ptr<Element> child){};

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

    Element* find_closest_relayout_boundary();
    Element* find_closest_repaint_boundary();

    // Parent element should set this during constructing and updating
    Element* parent = nullptr;

    // Document is set when this element is painted
    Document* document = nullptr;

  protected:
    // Is protected to allow ancestor of element class to access own tree
    std::shared_ptr<LayerTree> layer_tree;

  private:
    // Whether the element was changed by updating props or performig relayout
    // since last repaint.
    bool is_changed;

    // Absolute position is calculated before painting the element
    Position abs_position;

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

    std::shared_ptr<Element> child;
    void append_child(std::shared_ptr<Element> child) override;
    void remove_child(std::shared_ptr<Element> child) override;
    void visit_children(ChildrenVisitor visitor) override { visitor(child); };
};

class MultipleChildrenElement : public Element {
  public:
    MultipleChildrenElement(std::vector<std::shared_ptr<Element>> children,
                            bool is_repaint_boundary,
                            bool size_depends_on_parent);

    std::vector<std::shared_ptr<Element>> children;
    void remove_child(std::shared_ptr<Element> child) override;
    void append_child(std::shared_ptr<Element> child) override;
    void insert_before_child(std::shared_ptr<Element> child) override;
    void visit_children(ChildrenVisitor visitor) override {
        for (auto child : children) {
            visitor(child);
        }
    };
};

};  // namespace aardvark
