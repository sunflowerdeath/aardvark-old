#include <Catch2/catch.hpp>
#include <aardvark/box_constraints.hpp>

using namespace aardvark;

TEST_CASE("BoxConstraints", "[box_constraints]") {
    SECTION("make_loose") {
        auto tight = BoxConstraints{/* min_width */ 10,
                                    /* max_width */ 10,
                                    /* min_height */ 20,
                                    /* max_height */ 20};
        auto loose = tight.make_loose();
        REQUIRE(loose.min_width == 0);
        REQUIRE(loose.max_width == 10);
        REQUIRE(loose.min_height == 0);
        REQUIRE(loose.max_height == 20);
    }

    SECTION("is_tight") {
        auto not_tight = BoxConstraints{/* min_width */ 10,
                                        /* max_width */ 20,
                                        /* min_height */ 10,
                                        /* max_height */ 20};
        REQUIRE(!not_tight.is_tight());
        auto tight = BoxConstraints{/* min_width */ 10,
                                    /* max_width */ 10,
                                    /* min_height */ 20,
                                    /* max_height */ 20};
        REQUIRE(tight.is_tight());
    }

    SECTION("max_size") {
        auto constraints = BoxConstraints{/* min_width */ 10,
                                          /* max_width */ 20,
                                          /* min_height */ 10,
                                          /* max_height */ 20};
        auto size = constraints.max_size();
        REQUIRE(size.width == 20);
        REQUIRE(size.height == 20);
    }

    SECTION("from_size") {
        auto size = Size{10, 20};
        auto tight = BoxConstraints::from_size(size, /* tight */ true);
        REQUIRE(tight.min_width == 10);
        REQUIRE(tight.max_width == 10);
        REQUIRE(tight.min_height == 20);
        REQUIRE(tight.max_height == 20);
        auto loose = BoxConstraints::from_size(size, /* tight */ false);
        REQUIRE(loose.min_width == 0);
        REQUIRE(loose.max_width == 10);
        REQUIRE(loose.min_height == 0);
        REQUIRE(loose.max_height == 20);
    }
}
