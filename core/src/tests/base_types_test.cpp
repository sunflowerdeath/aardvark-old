#include "Catch2/catch.hpp"
#include "../base_types.hpp"

using namespace aardvark;

TEST_CASE("Size", "[size]" ) {
    SECTION("==") {
      REQUIRE(Size{10, 10} == Size{10, 10});
      REQUIRE(Size{10, 10} != Size{10, 5});
    }
}

TEST_CASE("Position", "[position]") {
    SECTION("==") {
        REQUIRE(Position{10, 10} == Position{10, 10});
        REQUIRE(Position{10, 10} != Position{10, 5});
    }

    SECTION("add") {
        REQUIRE(Position{5, 10} + Position{2, 3} == Position{7, 13});
    }
}

TEST_CASE("Value", "[value]") {
    auto total = 10;
    auto abs = Value::abs(1);
    auto rel = Value::rel(0.5);
    auto none = Value::none();

    SECTION("calc") {
        REQUIRE(abs.calc(total) == 1);
        REQUIRE(rel.calc(total) == 5);
        REQUIRE(none.calc(total) == 0);
    }

    SECTION("is_none") {
        REQUIRE(!abs.is_none());
        REQUIRE(!rel.is_none());
        REQUIRE(none.is_none());
    }
}
