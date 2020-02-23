#pragma once

#include "../base_types.hpp"
#include "../element.hpp"

namespace aardvark {

class LayerElement : public SingleChildElement {
  public:
    LayerElement()
        : SingleChildElement(
              /* child */ nullptr,
              /* is_repaint_boundary */ true,
              /* size_depends_on_parent */ false){};

    LayerElement(std::shared_ptr<Element> child, Transform transform)
        : SingleChildElement(
              std::move(child),
              /* is_repaint_boundary */ true,
              /* size_depends_on_parent */ false) {
        set_transform(transform);
    };

    std::string get_debug_name() override { return "Layer"; };

    Size layout(BoxConstraints constraints) override;

    void change_layer() {
        if (document != nullptr) document->change_layer(this);
    }

    void set_transform(const Transform& new_transform) {
        transform = new_transform;
        layer_tree->transform = new_transform.to_sk_matrix();
        change_layer();
    }

    void set_opacity(float new_opacity) {
        opacity = new_opacity;
        layer_tree->opacity = opacity;
        change_layer();
    }

    Transform transform;
    float opacity = 1;
};

}  // namespace aardvark
