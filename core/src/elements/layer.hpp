#pragma once

#include <memory>
#include <variant>
#include "SkMatrix.h"
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"

namespace aardvark::elements {

struct TransformOptions {
  Position translate;
  Scale scale;
  float opacity = 1;
  float rotation = 0;
};

class Layer : public Element {
 public:
  Layer(std::shared_ptr<Element> child,
        std::variant<SkMatrix, TransformOptions> transform);
  std::shared_ptr<Element> child;
	bool sized_by_parent = true;
  std::variant<SkMatrix, TransformOptions> transform;
  std::string get_debug_name() override { return "Layer"; };
  Size layout(BoxConstraints constraints) override;
  void paint(bool is_changed) override;
};

}  // namespace aardvark::elements
