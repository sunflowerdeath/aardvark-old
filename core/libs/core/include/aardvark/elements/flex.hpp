#pragma once

#include <optional>
#include "../element.hpp"

namespace aardvark::elements {

enum class FlexDirection { row, column };

enum class FlexAlign { start, center, end, stretch };

enum class FlexJustify {
    start,
    center,
    end,
    space_around,
    space_between,
    space_evenly
};

class Flex : public MultipleChildrenElement {
  public:
    Flex()
        : MultipleChildrenElement(/* children */ {},
                                  /* is_repaint_boundary */ false,
                                  /* size_depends_on_parent */ false){};

    Flex(std::vector<std::shared_ptr<Element>> children,
         FlexDirection direction, FlexJustify justify, FlexAlign align)
        : MultipleChildrenElement(/* children */ children,
                                  /* is_repaint_boundary */ false,
                                  /* size_depends_on_parent */ false),
          direction(direction),
          justify(justify),
          align(align){};

    std::string get_debug_name() override { return "Flex"; };
    float get_intrinsic_height() override;
    float get_intrinsic_width() override;
    Size layout(BoxConstraints constraints) override;
    HitTestMode get_hit_test_mode() override { return HitTestMode::Disabled; };

    // Which axis is main
    FlexDirection direction = FlexDirection::row;

    // Alignment of the children across the main axis
    FlexJustify justify = FlexJustify::start;

    // Alignment of the children across the secondary axis
    FlexAlign align = FlexAlign::start;
};

class FlexChild : public SingleChildElement {
  public:
    FlexChild()
        : SingleChildElement(/* child */ nullptr,
                             /* is_repaint_boundary */ false,
                             /* size_depends_on_parent */ false){};

    FlexChild(std::shared_ptr<Element> child, std::optional<FlexAlign> align,
              int flex, bool tight_fit)
        : SingleChildElement(/* child */ child,
                             /* is_repaint_boundary */ false,
                             /* size_depends_on_parent */ false),
          align(align),
          flex(flex),
          tight_fit(tight_fit){};

    std::string get_debug_name() override { return "FlexChild"; };
    Size layout(BoxConstraints constraints) override;
    HitTestMode get_hit_test_mode() override { return HitTestMode::Disabled; };

    // Flex grow factor
    int flex = 0;

    // Overrides align property of the container
    std::optional<FlexAlign> align = FlexAlign::start;

    // Whether to force flexible child to take all of the provided space
    bool tight_fit = true;
};

}  // namespace aardvark::elements
