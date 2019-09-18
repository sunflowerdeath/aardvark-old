#pragma once

#include <memory>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"

namespace aardvark {

struct Padding {
    Value left = Value::none();
    Value top = Value::none();
    Value right = Value::none();
    Value bottom = Value::none();
};

class PaddingElement : public SingleChildElement {
  public:
    PaddingElement()
        : SingleChildElement(/* child */ nullptr,
                             /* is_repaint_boundary */ false,
                             /* size_depends_on_parent */ false){};

    PaddingElement(std::shared_ptr<Element> child, Padding padding,
                   bool is_repaint_boundary = false)
        : SingleChildElement(child, is_repaint_boundary,
                             /* size_depends_on_parent */ false),
          padding(padding){};

    std::string get_debug_name() override { return "Padding"; };
    float get_intrinsic_height() override;
    float get_intrinsic_width() override;
    Size layout(BoxConstraints constraints) override;

    Padding padding;
};

}  // namespace aardvark
