#include "Catch2/catch.hpp"
#include "../../js/helpers.hpp"
#include "../../js/mappers.hpp"
#include "../../js/base_types_mappers.hpp"

using namespace aardvark;
using namespace aardvark::js;

TEST_CASE("Mappers", "[mappers]" ) {
    SECTION("FunctionMapper") {
        auto ctx = JsGlobalContextWrapper::make();
        auto src = JSStringCreateWithUTF8CString(
            "(text, number) => text + String(number)");
        auto js_func = JSEvaluateScript(ctx->get(),  // ctx,
                                        src,         // script
                                        nullptr,     // thisObject,
                                        nullptr,     // sourceURL,
                                        1,           // startingLineNumber,
                                        nullptr      // exception
        );
        auto mapper = FunctionMapper<std::string, std::string, float>(
            str_mapper, str_mapper, float_mapper);
        auto func = mapper.from_js(ctx->get(), js_func);
        auto res = func("hello", 10);
        REQUIRE(res == "hello10");
    }
}
