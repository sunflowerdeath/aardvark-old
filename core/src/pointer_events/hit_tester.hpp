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
    void test(float left, float top);
    std::vector<std::shared_ptr<Element>> hit_elements;
  private:
    Document* document;
    void test_element(std::shared_ptr<Element> elem, float left, float top);
    SkMatrix transform;
};

}
