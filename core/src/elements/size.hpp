#pragma once

#include <memory>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"

namespace aardvark::elements {

struct ASize {
    Value min_width = Value::none();
    Value max_width = Value::none();
    Value min_height = Value::none();
    Value max_height = Value::none();

    static ASize exact(Value width, Value height) {
        return ASize{width, width, height, height};
    }
};

class SizeElement : public SingleChildElement {
  public:
    SizeElement(std::shared_ptr<Element> child, ASize size,
                bool is_repaint_boundary = false);
    ASize size;
    std::string get_debug_name() override { return "Size"; };
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;
};

}  // namespace aardvark::elements
