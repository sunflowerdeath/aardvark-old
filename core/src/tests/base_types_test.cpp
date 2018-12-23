#define CATCH_CONFIG_MAIN
#include "catch.hpp"

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

