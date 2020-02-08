#include "Catch2/catch.hpp"
#include "../base_types.hpp"
#include "../elements/elements.hpp"

using namespace aardvark;

TEST_CASE("Align", "[align]") {
    auto left_top_insets = elements::EdgeInsets{
        Value::abs(20),  // left
        Value::abs(30),  // top
    };
    auto bottom_right_insets = elements::EdgeInsets{
        Value::none(),   // left
        Value::none(),   // top
        Value::abs(20),  // right
        Value::abs(30),  // bottom
    };
    auto padding_insets = elements::EdgeInsets::all(Value::abs(10));
    auto rel_insets = elements::EdgeInsets{
        Value::rel(0.2),  // left
        Value::rel(0.5),  // top
    };

    auto constraints = BoxConstraints{
        0,    // min_width
        100,  // max_width
        0,    // min_height,
        200   // max_height
    };

    SECTION("left_top") {
        auto child = std::make_shared<elements::Background>(SK_ColorRED);
        auto align = std::make_shared<elements::Align>(child, left_top_insets);
        auto size = align->layout(constraints);

        REQUIRE(size == Size{100, 200});
        REQUIRE(child->rel_position == Position{20, 30});
        REQUIRE(child->size == Size{80, 170});
    }

    SECTION("bottom_right") {
        auto child = std::make_shared<elements::Background>(SK_ColorRED);
        auto align =
            std::make_shared<elements::Align>(child, bottom_right_insets);
        auto size = align->layout(constraints);

        REQUIRE(size == Size{100, 200});
        REQUIRE(child->rel_position == Position{0, 0});
        REQUIRE(child->size == Size{80, 170});
    }

    SECTION("padding") {
        auto child = std::make_shared<elements::Background>(SK_ColorRED);
        auto align = std::make_shared<elements::Align>(child, padding_insets);
        auto size = align->layout(constraints);

        REQUIRE(size == Size{100, 200});
        REQUIRE(child->rel_position == Position{10, 10});
        REQUIRE(child->size == Size{80, 180});
    }

    SECTION("rel") {
        auto child = std::make_shared<elements::Background>(SK_ColorRED);
        auto align = std::make_shared<elements::Align>(child, rel_insets);
        auto size = align->layout(constraints);

        REQUIRE(size == Size{100, 200});
        INFO(child->rel_position.left);
        INFO(child->rel_position.top);
        REQUIRE(child->rel_position == Position{100 * 0.2, 200 * 0.5});
    }
}
