#include "element.hpp"

namespace aardvark {

Element::Element(bool is_repaint_boundary) {
  this->is_repaint_boundary = is_repaint_boundary;
};

bool Element::is_parent_of(Element* elem) {
  auto current = elem->parent;
  while (current) {
    if (current == this) return true;
    current = current->parent;
  }
  return false;
};

Element* Element::find_closest_relayout_boundary() {
  if (this->is_relayout_boundary) return this;
  auto current = this->parent;
  while (!current->is_relayout_boundary) current = current->parent;
  return current;
};

Element* Element::find_closest_repaint_boundary() {
  if (this->is_repaint_boundary) return this;
  auto current = this->parent;
  while (!current->is_repaint_boundary) current = current->parent;
  return current;
};

}; // namepsace aardvark
