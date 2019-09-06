#include "Catch2/catch.hpp"
#include <memory>
#include <optional>
#include "JavaScriptCore/JavaScript.h"
#include "../utils/event_loop.hpp"
#include "../js/module_loader.hpp"

using namespace aardvark;
using namespace aardvark::js;

TEST_CASE("ModuleLoader", "[module_loader]" ) {
    auto event_loop = EventLoop();
    auto ctx = JSGlobalContextCreate(nullptr);
    auto ctx_sptr = std::make_shared<JSGlobalContextWrapper>(ctx);

    SECTION("load module and get result") {
        auto loader = ModuleLoader(&event_loop, ctx_sptr, false);
        auto source = "2 + 2";
        auto result = loader.load_from_source(source);
        REQUIRE(JSValueToNumber(ctx, result, nullptr) == 4);
    }

    SECTION("load module and handle exception") {
        auto loader = ModuleLoader(&event_loop, ctx_sptr, false);
        std::optional<JsError> error = std::nullopt;
        loader.exception_handler = [&error](JsError error_arg) {
            error = error_arg;
        };

        auto source = "2 + a";
        auto source_url = "SOURCE_URL";
        auto result = loader.load_from_source(source, source_url);

        REQUIRE(JSValueIsNull(ctx, result));
        REQUIRE(error != std::nullopt);
        REQUIRE(error->location.source_url == source_url);
        REQUIRE(error->location.line != -1);
        REQUIRE(error->location.column != -1);
    }

    /*
    SECTION("inline source map in file") {
        auto loader = ModuleLoader(&event_loop, ctx, true);
        std::optional<JsError> error = std::nullopt;
        loader.exception_handler = [&error](JsError error_arg) {
            error = error_arg;
        };

        auto result = loader.load_from_file("fixtures/inline-source-map.js");

        REQUIRE(JSValueIsNull(ctx, result));
        std::cout << "ERROR" << error->location.source_url << std::endl;
    }
    */

    SECTION("external source map in file") {
        auto loader = ModuleLoader(&event_loop, ctx_sptr, true);
        std::optional<JsError> error = std::nullopt;
        loader.exception_handler = [&error](JsError error_arg) {
            error = error_arg;
        };

        auto result = loader.load_from_file("fixtures/external-source-map.js");

        REQUIRE(JSValueIsNull(ctx, result));
        REQUIRE(error != std::nullopt);
        REQUIRE(error->original_location.source_url ==
                "webpack:///./src/index.js");
        REQUIRE(error->original_location.line == 1);
        REQUIRE(error->original_location.column == 0);
    }
}
