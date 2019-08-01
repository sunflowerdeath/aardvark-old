#include "element.hpp"

namespace aardvark {

Element::Element(bool is_repaint_boundary, bool size_depends_on_parent) {
  this->is_repaint_boundary = is_repaint_boundary;
  this->size_depends_on_parent = size_depends_on_parent;
};

void Element::change() {
  if (document != nullptr) document->change_element(this);
}

bool Element::hit_test(double left, double top) {
  return (left >= abs_position.left &&
          left <= abs_position.left + size.width && top >= abs_position.top &&
          top <= abs_position.top + size.height);
};

HitTestMode Element::get_hit_test_mode() {
  return HitTestMode::PassToParent;
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

SingleChildElement::SingleChildElement(std::shared_ptr<Element> child,
                                       bool is_repaint_boundary,
                                       bool size_depends_on_parent)
    : child(child), Element(is_repaint_boundary, size_depends_on_parent) {
  child->parent = this;
};

void SingleChildElement::remove_child(std::shared_ptr<Element> child) {
  if (this->child == child) {
    this->child->parent = nullptr;
    this->child = nullptr;
    change();
  }
};

void SingleChildElement::append_child(std::shared_ptr<Element> child) {
  this->child = child;
  change();
};

MultipleChildrenElement::MultipleChildrenElement(
    std::vector<std::shared_ptr<Element>> children, bool is_repaint_boundary,
    bool size_depends_on_parent)
    : children(children), Element(is_repaint_boundary, size_depends_on_parent) {
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

}; // namespace aardvark
