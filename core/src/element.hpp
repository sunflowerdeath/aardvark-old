#pragma once

#include <memory>
#include "document.hpp"
#include "base_types.hpp"
#include "box_constraints.hpp"

namespace aardvark {

// forward declaration due to circular includes
class Document;
class LayerTree;

class Element {
 public:
  Element(bool isRepaintBoundary);
  Document* document;
  std::shared_ptr<LayerTree> layerTree;
  Element* parent;
  Size size;
  Position relPosition;
  Position absPosition;
  bool sizedByParent = true;
  bool isRelayoutBoundary = false;
  bool isRepaintBoundary = false;
  BoxConstraints prevConstraints;
  virtual Size layout(BoxConstraints constraints);
  virtual void paint();
  bool isParentOf(Element* elem);
  Element* findClosestRelayoutBoundary();
  Element* findClosestRepaintBoundary();
};

};  // namespace aardvark
