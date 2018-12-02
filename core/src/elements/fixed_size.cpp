#include "fixed_size.hpp"

namespace aardvark::elements {

FixedSize::FixedSize(std::shared_ptr<Element> child, Size size,
                     bool isRepaintBoundary)
    : Element(isRepaintBoundary), size(size), child(child) {
  child->parent = this;
};

Size FixedSize::layout(BoxConstraints constraints) {
	document->layoutElement(child.get(), BoxConstraints::fromSize(size));
	child->size = size;
	child->relPosition = Position{0 /* left */, 0 /* top */};
	return size;
};

void FixedSize::paint() { document->paintElement(child.get()); };

}  // namespace aardvark::elements
