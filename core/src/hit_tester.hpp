#pragma once

#include <vector>
#include <memory>
#include "SkMatrix.h"
#include "element.hpp"

namespace aardvark {

class Element;

class HitTester {
  public:
    void test(std::shared_ptr<Element> root, double left, double top);
    std::vector<std::shared_ptr<Element>> hit_elements;
  private:
    void test_element(std::shared_ptr<Element> elem, double left, double top);
    SkMatrix transform;
};

}
