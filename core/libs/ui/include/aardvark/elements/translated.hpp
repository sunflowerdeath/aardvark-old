#pragma once

#include <memory>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"

namespace aardvark {

struct Translation {
    Value left = Value::none();
    Value top = Value::none();
};

class TranslatedElement : public SingleChildElement {
  public:
    TranslatedElement()
        : SingleChildElement(
              /* child */ nullptr,
              /* is_repaint_boundary */ false,
              /* size_depends_on_parent */ false){};

    TranslatedElement(
        std::shared_ptr<Element> child,
        Translation translation,
        bool is_repaint_boundary = false)
        : translation(translation),
          SingleChildElement(
              std::move(child),
              is_repaint_boundary,
              /* size_depends_on_parent */ false){};

    std::string get_debug_name() override { return "Translate"; };
    HitTestMode get_hit_test_mode() override { return HitTestMode::Disabled; };
    float get_intrinsic_height(float width) override;
    float get_intrinsic_width(float height) override;
    Size layout(BoxConstraints constraints) override;
    bool hit_test(double left, double top) override;

    ELEMENT_PROP(Translation, translation);
};

}  // namespace aardvark
