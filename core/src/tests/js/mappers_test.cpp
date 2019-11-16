#include "Catch2/catch.hpp"
#include "../../js/mappers.hpp"
#include "../../js/base_types_mappers.hpp"

using namespace aardvark;
using namespace aardvark::js;

TEST_CASE("Mappers", "[mappers]" ) {
    SECTION("FunctionMapper") {
        auto ctx = JSGlobalContextCreate(nullptr);

        auto source = "(number, text) => String(number) + text";
        auto js_source = JSStringCreateWithUTF8CString(source);
        auto function = JSEvaluateScript(ctx,        // ctx,
                                         js_source,  // script
                                         nullptr,    // thisObject,
                                         nullptr,    // sourceURL,
                                         1,          // startingLineNumber,
                                         nullptr     // exception
        );

        auto mapper = FunctionMapper<std::string, float, std::string>(
            ctx, function, str_mapper, float_mapper, str_mapper);

        auto values = mapper.args_to_js(ctx, 5, "hello");
        REQUIRE(values.size() == 2);
        REQUIRE(float_mapper->from_js(ctx, values[0]) == 5);
        REQUIRE(str_mapper->from_js(ctx, values[1]) == "hello");

        // REQUIRE(Size{10, 10} == Size{10, 10});
    }
}
