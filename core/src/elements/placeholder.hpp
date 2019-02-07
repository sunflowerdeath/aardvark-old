#pragma once

#include <memory>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"

namespace aardvark::elements {

class Placeholder : public Element {
  public:
    Placeholder()
        : Element(/* is_repaint_boundary */ false,
                  /* size_depends_on_parent */ true){};
    std::string get_debug_name() override { return "Placeholder"; };
    Size layout(BoxConstraints constraints) override {
        return constraints.max_size();
    };
    void paint(bool is_changed) override{};
};

}  // namespace aardvark::elements
