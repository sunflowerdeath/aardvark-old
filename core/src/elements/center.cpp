#include "center.hpp"

namespace aardvark::elements {

Center::Center(std::shared_ptr<Element> child, bool isRepaintBoundary)
    : Element(isRepaintBoundary), child(child) {
  child->parent = this;
};

Size Center::layout(BoxConstraints constraints) {
  auto childConstraints = BoxConstraints{
      0,                     // minWidth
      constraints.maxWidth,  // maxWidth
      0,                     // minHeight
      constraints.maxHeight  // maxHeight
  };
  auto childSize = document->layoutElement(child.get(), childConstraints);
  child->size = childSize;
  child->relPosition = Position{
      (constraints.maxWidth - childSize.width) / 2,   // left
      (constraints.maxHeight - childSize.height) / 2  // top
  };
  return Size{
      constraints.maxWidth,  // width
      constraints.maxHeight  // height
  };
};

void Center::paint() { document->paintElement(child.get()); };

}  // namespace aardvark::elements
