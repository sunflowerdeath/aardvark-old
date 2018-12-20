#include "element.hpp"

namespace aardvark {

Element::Element(bool is_repaint_boundary) {
  this->is_repaint_boundary = is_repaint_boundary;
};

void Element::change() {
  if (document != nullptr) document->change_element(this);
}

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

SingleChildElement::SingleChildElement(std::shared_ptr<Element> child,
                                       bool is_repaint_boundary)
    : child(child), Element(is_repaint_boundary) {
  child->parent = this;
};

void SingleChildElement::remove_child(std::shared_ptr<Element> child) {
  if (this->child == child) {
    this->child = nullptr;
    change();
  }
};

void SingleChildElement::insert_child(std::shared_ptr<Element> child) {
  this->child = child;
  change();
};

MultipleChildrenElement::MultipleChildrenElement(
    std::vector<std::shared_ptr<Element>> children, bool is_repaint_boundary)
    : children(children), Element(is_repaint_boundary) {
  for (auto child : children) child->parent = this;
};

void MultipleChildrenElement::remove_child(std::shared_ptr<Element> child) {
  auto it = std::find(children.begin(), children.end(), child);
  if (it != children.end()) {
    child->parent = nullptr;
    children.erase(it);
    change();
  }
};

void MultipleChildrenElement::append_child(std::shared_ptr<Element> child) {
  child->parent = this;
  children.push_back(child);
  change();
};

void MultipleChildrenElement::insert_before_child(
    std::shared_ptr<Element> child) {
  child->parent = this;
  auto it = std::find(children.begin(), children.end(), child);
  children.insert(it, child);
  change();
};

}; // namepsace aardvark
