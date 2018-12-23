#pragma once

#include <memory>
#include <variant>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"
#include "SkMatrix.h"

namespace aardvark::elements {

struct TransformOptions {
    Position translate;
    Scale scale;
    float opacity = 1;
    float rotation = 0;
};

class Layer : public SingleChildElement {
  public:
    Layer(std::shared_ptr<Element> child,
          std::variant<SkMatrix, TransformOptions> transform);

    std::variant<SkMatrix, TransformOptions> transform;
    std::string get_debug_name() override { return "Layer"; };
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;
};

}  // namespace aardvark::elements
