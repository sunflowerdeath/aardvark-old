#pragma once

#include "../base_types.hpp"
#include "../element.hpp"

namespace aardvark {

struct SizeConstraints {
    Value width = Value::none();
    Value height = Value::none();
    Value min_width = Value::none();
    Value max_width = Value::none();
    Value min_height = Value::none();
    Value max_height = Value::none();

    static SizeConstraints exact(Value width, Value height) {
        return SizeConstraints{width, width, height, height};
    }
};

class SizeElement : public SingleChildElement {
  public:
    SizeElement()
        : SingleChildElement(
              nullptr,
              /* is_repaint_boundary */ false,
              /* size_depends_on_parent */ false){};

    SizeElement(
        std::shared_ptr<Element> child,
        SizeConstraints size_constraints,
        bool is_repaint_boundary = false)
        : SingleChildElement(
              std::move(child),
              is_repaint_boundary,
              /* size_depends_on_parent */ false),
          size_constraints(size_constraints){};

    std::string get_debug_name() override { return "Sized"; };
    float get_intrinsic_height() override;
    float get_intrinsic_width() override;
    Size layout(BoxConstraints constraints) override;

    ELEMENT_PROP_WITH_SETTER(SizeConstraints, size_constraints);
};

}  // namespace aardvark::elements
