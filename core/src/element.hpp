#pragma once

#include <memory>
#include <string>
#include "document.hpp"
#include "base_types.hpp"
#include "box_constraints.hpp"

namespace aardvark {

// forward declaration due to circular includes
class Document;
class LayerTree;

// Base class for elements of the document
class Element {
 public:
  Element(bool is_repaint_boundary);
  
  // Document is set when this element is painted
  Document* document = nullptr;

  // !
  // Parent element should set this during constructing and updating
  Element* parent = nullptr;
 
  // These props should be set during layout by the parent element
  Size size;
  Position rel_position;

  // !
  // Absolute position is calculated before painting the element
  Position abs_position;

  // Element should set this to true when its size depends only on 
  // input constraints
  bool sized_by_parent = true;

  // !
  // When element is relayout boundary, changes inside it do not affect 
  // layout of parents. This can happen when element recieves tight
  // constraints, so it is always same size, or when element's size
  // depends only on input constraints.
  // This prop is set automatically depending on the given layout constraints
  // and value of the `sized_by_parent`.
  bool is_relayout_boundary = false;

  // Repaint boundary element does not share layers with another elements.
  // This allows to repaint this element separately.
  // This prop should be set explicitly.
  bool is_repaint_boundary = false;

  // !
  std::shared_ptr<LayerTree> layer_tree;

  // !
  // This is used for relayout
  BoxConstraints prev_constraints;

  // In this method element should calculate its size, layout children 
  // and set their size and relative positions.
  virtual Size layout(BoxConstraints constraints) {};

  // !
  // Whether the element was changed by updating props or performig relayout
  // since last repaint.
  bool is_changed;

  // Paints element and its children.
  // `is_changed` is `true` when the element itself or some of its parents 
  // is changed. When it is `false`, element is allowed to reuse result of
  // previous painting.
  virtual void paint(bool is_changed) {};

  // Used for debugging
  virtual std::string get_debug_name() { return "Unknown element"; };

  void change();

  // Check whether the element is direct or indirect parent of another element
  bool is_parent_of(Element* elem);

  Element* find_closest_relayout_boundary();
  Element* find_closest_repaint_boundary();
};

class SingleChildElement : public Element {
 public:
  SingleChildElement(std::shared_ptr<Element> child, bool is_repaint_boundary);
  std::shared_ptr<Element> child;
  void remove_child(std::shared_ptr<Element> child);
  void insert_child(std::shared_ptr<Element> child);
};

class MultipleChildrenElement : public Element {
 public:
  MultipleChildrenElement(std::vector<std::shared_ptr<Element>> children,
                          bool is_repaint_boundary);
  std::vector<std::shared_ptr<Element>> children;
  void remove_child(std::shared_ptr<Element> child);
  void append_child(std::shared_ptr<Element> child);
  void insert_before_child(std::shared_ptr<Element> child);
};

};  // namespace aardvark
