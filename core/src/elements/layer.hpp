#pragma once

#include <memory>
#include <variant>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"
#include "SkMatrix.h"

namespace aardvark::elements {

/*
struct TransformOptions {
    Position translate;
    Scale scale;
    float opacity = 1;
    float rotation = 0;
};
*/

class Layer : public SingleChildElement {
  public:
    Layer()
        : SingleChildElement(/* child */ nullptr,
                             /* is_repaint_boundary */ true,
                             /* size_depends_on_parent */ true){};

    Layer(std::shared_ptr<Element> child, SkMatrix transform)
        : SingleChildElement(child, /* is_repaint_boundary */ true,
                             /* size_depends_on_parent */ true),
          transform(transform){};

    SkMatrix transform = SkMatrix::MakeScale(1);
    std::string get_debug_name() override { return "Layer"; };
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;
};

}  // namespace aardvark::elements
