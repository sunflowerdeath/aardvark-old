#pragma once

#include <vector>
#include <memory>
#include "SkMatrix.h"
#include "../element.hpp"
#include "../document.hpp"

namespace aardvark {

class Element;
class Document;

class HitTester {
  public:
    HitTester(Document* document) : document(document){};
    std::vector<std::weak_ptr<Element>> test(float left, float top);

  private:
    Document* document;
    void test_element(std::shared_ptr<Element> elem, float left, float top);
    SkMatrix transform;
    std::vector<std::shared_ptr<Element>> elements_under_pointer;
};

}
