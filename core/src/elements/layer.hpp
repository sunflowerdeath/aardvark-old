#pragma once

#include <memory>
#include <variant>

#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"
#include "SkMatrix.h"

namespace aardvark::elements {

class Layer : public SingleChildElement {
  public:
    Layer()
        : SingleChildElement(/* child */ nullptr,
                             /* is_repaint_boundary */ true,
                             /* size_depends_on_parent */ false){};

    Layer(std::shared_ptr<Element> child, SkMatrix transform)
        : SingleChildElement(child, /* is_repaint_boundary */ true,
                             /* size_depends_on_parent */ false) {
        set_transform(transform);
    };

    std::string get_debug_name() override { return "Layer"; };

    Size layout(BoxConstraints constraints) override;

    void change_layer() {
        if (document != nullptr) document->change_layer(this);
    }

    void set_transform(const SkMatrix& new_transform) {
        transform = new_transform;
        layer_tree->transform = new_transform;
        change_layer();
    }

    SkMatrix get_transform() { return transform; }

    void set_opacity(float new_opacity) {
        opacity = new_opacity;
        layer_tree->opacity = opacity;
        change_layer();
    }

    float get_opacity() { return opacity; }

  private:
    SkMatrix transform = SkMatrix::MakeScale(1);
    float opacity = 1;
};

}  // namespace aardvark::elements
