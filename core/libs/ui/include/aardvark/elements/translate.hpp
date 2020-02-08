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

class TranslateElement : public SingleChildElement {
  public:
    TranslateElement()
        : SingleChildElement(/* child */ nullptr,
                             /* is_repaint_boundary */ false,
                             /* size_depends_on_parent */ false){};

    TranslateElement(std::shared_ptr<Element> child, Translation translation,
                     bool is_repaint_boundary = false)
        : SingleChildElement(child, is_repaint_boundary,
                             /* size_depends_on_parent */ false){};

    std::string get_debug_name() override { return "Translate"; };
    float get_intrinsic_height() override;
    float get_intrinsic_width() override;
    Size layout(BoxConstraints constraints) override;
    bool hit_test(double left, double top) override;

    Translation translation;
};

}  // namespace aardvark
