#include "aardvark_js/module_loader.hpp"
#include <iostream>

#include <Catch2/catch.hpp>
#include <optional>

#include "aardvark/utils/event_loop.hpp"
#include "aardvark_jsi/jsi.hpp"
#include "aardvark_jsi/qjs.hpp"

using namespace aardvark;

TEST_CASE("ModuleLoader", "[module_loader]" ) {
    auto event_loop = EventLoop();
    auto ctx = jsi::Qjs_Context::create();

    SECTION("load_from_source") {
        auto loader = js::ModuleLoader(&event_loop, ctx.get(), false, nullptr);
        auto source = "2 + 2";
        auto result = loader.load_from_source(source);
        REQUIRE(result.to_number().value() == 4);
    }

    SECTION("error_handler") {
        std::optional<jsi::Error> err = std::nullopt;
        auto error_handler = [&err](
                                 jsi::Error& err_arg,
                                 std::optional<jsi::ErrorLocation> orig_loc) {
            err = err_arg;
        };
        auto loader =
            js::ModuleLoader(&event_loop, ctx.get(), false, error_handler);

        auto source = "2 + a";
        auto source_url = "SOURCE_URL";
        auto res = loader.load_from_source(source, source_url);

        REQUIRE(res.get_type() == jsi::ValueType::null);
        REQUIRE(err.has_value());
        auto loc = err.value().location();
        REQUIRE(loc.has_value());
        REQUIRE(loc.value().source_url == source_url);
        // REQUIRE(loc.value().line != -1);
        // REQUIRE(err->location.column != -1);
    }

    /* TODO
    SECTION("inline source map") {
        std::optional<jsi::Error> err = std::nullopt;
        std::optional<jsi::ErrorLocation> orig_loc = std::nullopt;
        auto error_handler =
            [&](
                jsi::Error& err_arg,
                std::optional<jsi::ErrorLocation> orig_loc_arg) {
                err = err_arg;
                orig_loc = orig_loc_arg;
            };
        auto loader =
            js::ModuleLoader(&event_loop, ctx.get(), true, error_handler);

        auto res =
            loader.load_from_file("adv_js_tests_fixtures/inline-source-map.js");

        REQUIRE(res.get_type() == jsi::ValueType::null);
        REQUIRE(orig_loc.value().source_url == "webpack:///./src/index.js");
        REQUIRE(orig_loc.value().line == 1);
        REQUIRE(orig_loc.value().column == 0);
    }
    */

    SECTION("external source map") {
        std::optional<jsi::Error> err = std::nullopt;
        std::optional<jsi::ErrorLocation> orig_loc = std::nullopt;
        auto error_handler =
            [&](
                jsi::Error& err_arg,
                std::optional<jsi::ErrorLocation> orig_loc_arg) {
                err = err_arg;
                orig_loc = orig_loc_arg;
            };
        auto loader =
            js::ModuleLoader(&event_loop, ctx.get(), true, error_handler);

        auto res = loader.load_from_file(
            "adv_js_tests_fixtures/external-source-map.js");

        REQUIRE(res.get_type() == jsi::ValueType::null);
        REQUIRE(orig_loc.value().source_url == "webpack:///./src/index.js");
        REQUIRE(orig_loc.value().line == 1);
        REQUIRE(orig_loc.value().column == 0);
    }
}
