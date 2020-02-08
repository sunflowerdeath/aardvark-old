#pragma once

#include <memory>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"

namespace aardvark::elements {

class Center : public SingleChildElement {
  public:
    Center()
        : SingleChildElement(nullptr, /* is_repaint_boundary */ false,
                             /* size_depends_on_parent */ true){};

    Center(std::shared_ptr<Element> child, bool is_repaint_boundary)
        : SingleChildElement(child, is_repaint_boundary,
                             /* size_depends_on_parent */ true){};

    std::string get_debug_name() override { return "Center"; };
    Size layout(BoxConstraints constraints) override;
    HitTestMode get_hit_test_mode() override { return HitTestMode::Disabled; };
};

}  // namespace aardvark::elements
