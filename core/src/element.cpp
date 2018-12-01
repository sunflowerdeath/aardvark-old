#include "element.hpp"

namespace aardvark {

Element::Element(bool isRepaintBoundary) {
  this->isRepaintBoundary = isRepaintBoundary;
};

bool Element::isParentOf(Element* elem) {
  auto current = elem->parent;
  while (current) {
    if (current == this) return true;
    current = current->parent;
  }
  return false;
};

Element* Element::findClosestRelayoutBoundary() {
  if (this->isRelayoutBoundary) return this;
  auto current = this->parent;
  while (!current->isRelayoutBoundary) current = current->parent;
  return current;
};

Element* Element::findClosestRepaintBoundary() {
  if (this->isRepaintBoundary) return this;
  auto current = this->parent;
  while (!current->isRepaintBoundary) current = current->parent;
  return current;
};

}; // namepsace aardvark
