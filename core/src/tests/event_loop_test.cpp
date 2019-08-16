#include "../utils/event_loop.hpp"
#include <iostream>
#include "Catch2/catch.hpp"

using namespace aardvark;

TEST_CASE("EventLoop", "[event_loop]") {
    SECTION("timeout") {
        auto loop = EventLoop();

        int timeout1;
        int timeout2;

        auto cb1_is_called = false;
        auto cb2_is_called = false;

        timeout1 = loop.set_timeout(
            [&]() {
                cb1_is_called = true;
                loop.clear_timeout(timeout2);
            },
            5);

        timeout2 =
            loop.set_timeout([&cb2_is_called]() { cb2_is_called = true; }, 10);

        std::this_thread::sleep_for(std::chrono::milliseconds(15));
        loop.poll();

        REQUIRE(cb1_is_called);
        REQUIRE(!cb2_is_called);
    }

    SECTION("callback") {
        auto loop = EventLoop();

        int cb1 = 0;
        int cb2 = 0;

        auto cb1_is_called = false;
        auto cb2_is_called = false;

        cb1 = loop.post_callback([&]() {
            loop.cancel_callback(cb2);
            cb1_is_called = true;
        });

        cb2 = loop.post_callback([&cb2_is_called]() { cb2_is_called = true; });

        loop.poll();

        REQUIRE(cb1_is_called);
        REQUIRE(!cb2_is_called);
    }
}
