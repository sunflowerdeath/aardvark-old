#include "element.hpp"

namespace aardvark {

Element::Element(bool is_repaint_boundary, bool size_depends_on_parent)
    : is_repaint_boundary(is_repaint_boundary),
      size_depends_on_parent(size_depends_on_parent),
      layer_tree(std::make_shared<LayerTree>(this)){};

void Element::change() {
    if (document != nullptr) document->change_element(this);
}

bool Element::hit_test(double left, double top) {
    return (left >= 0 && left <= size.width && top >= 0 && top <= size.height);
}

HitTestMode Element::get_hit_test_mode() { return HitTestMode::PassToParent; };

bool Element::is_parent_of(Element* elem) {
    auto current = elem->parent;
    while (current) {
        if (current == this) return true;
        current = current->parent;
    }
    return false;
}

void Element::set_document(Document* new_document) {
    if (document != new_document) {
        document = new_document;
        visit_children([new_document](std::shared_ptr<Element>& child) {
            child->set_document(new_document);
        });
    }
}

Element* Element::find_closest_relayout_boundary() {
    auto current = this;
    while (!current->is_relayout_boundary) current = current->parent;
    return current;
}

Element* Element::find_closest_repaint_boundary() {
    auto current = this;
    while (!current->is_repaint_boundary) current = current->parent;
    return current;
}

SingleChildElement::SingleChildElement(
    std::shared_ptr<Element> child,
    bool is_repaint_boundary,
    bool size_depends_on_parent)
    : child(std::move(child)),
      Element(is_repaint_boundary, size_depends_on_parent) {
    if (this->child) this->child->parent = this;
}

void SingleChildElement::paint(bool is_changed) {
    if (child != nullptr) document->paint_element(child.get());
}

void SingleChildElement::remove_child(std::shared_ptr<Element> child) {
    if (this->child == child) {
        // Change must be called before removing element, so the parent will
        // always replace the child in the `document->changed_elements`
        change();
        this->child->parent = nullptr;
        this->child->set_document(nullptr);
        this->child = nullptr;
    }
}

void SingleChildElement::append_child(std::shared_ptr<Element> child) {
    if (child != nullptr && child->parent != nullptr) child->parent->remove_child(child);
    this->child = child;
    this->child->set_document(this->document);
    this->child->parent = this;
    change();
}

MultipleChildrenElement::MultipleChildrenElement(
    std::vector<std::shared_ptr<Element>> children,
    bool is_repaint_boundary,
    bool size_depends_on_parent)
    : children(std::move(children)),
      Element(is_repaint_boundary, size_depends_on_parent) {
    for (auto& child : this->children) child->parent = this;
}

float MultipleChildrenElement::get_intrinsic_height(float width) {
    auto max_height = 0;
    for (auto& child : children) {
        auto height = child->query_intrinsic_height(width);
        if (height > max_height) max_height = height;
    }
    return max_height;
}

float MultipleChildrenElement::get_intrinsic_width(float height) {
    auto max_width = 0;
    for (auto& child : children) {
        auto width = child->query_intrinsic_width(height);
        if (width > max_width) max_width = width;
    }
    return max_width;
}

void MultipleChildrenElement::paint(bool is_changed) {
    for (auto child : children) {
        document->paint_element(child.get());
    }
}

void MultipleChildrenElement::remove_child(std::shared_ptr<Element> child) {
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        change();
        child->parent = nullptr;
        child->set_document(nullptr);
        children.erase(it);
    }
}

void MultipleChildrenElement::append_child(std::shared_ptr<Element> child) {
    if (child->parent != nullptr) child->parent->remove_child(child);
    child->parent = this;
    child->set_document(this->document);
    children.push_back(child);
    change();
}

void MultipleChildrenElement::insert_before_child(
    std::shared_ptr<Element> child, std::shared_ptr<Element> before_child) {
    if (child->parent != nullptr) child->parent->remove_child(child);
    auto it = std::find(children.begin(), children.end(), before_child);
    if (it == children.end()) return;
    child->parent = this;
    child->set_document(this->document);
    children.insert(it, child);
    change();
}

void MultipleChildrenElement::visit_children(ChildrenVisitor visitor) {
    for (auto& child : children) visitor(child);
}

}  // namespace aardvark
