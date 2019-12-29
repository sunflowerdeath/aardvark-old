#include <Catch2/catch.hpp>
#include <aardvark_jsi/check.hpp>
#include <aardvark_jsi/jsi.hpp>

#ifdef ADV_JSI_JSC
#include <aardvark_jsi/jsc.hpp>
#endif

#ifdef ADV_JSI_QJS
#include <aardvark_jsi/qjs.hpp>
#endif

using namespace aardvark::jsi;

void require_error(
    const Checker& checker, const Context& ctx, const Value& value) {
    auto result = checker(ctx, value, {"kind", "name", "target"});
    REQUIRE(result.has_value());
}

void require_valid(
    const Checker& checker, const Context& ctx, const Value& value) {
    auto result = checker(ctx, value, {"kind", "name", "target"});
    REQUIRE(!result.has_value());
}

TEMPLATE_TEST_CASE(
    "check",
    "[check]"
#ifdef ADV_JSI_QJS
    ,
    Qjs_Context
#endif
#ifdef ADV_JSI_JSC
    ,
    Jsc_Context
#endif
) {
    auto create_context = []() { return TestType::create(); };

    SECTION("primitive") {
        auto ctx = create_context();

        require_valid(number_checker, *ctx.get(), ctx->value_make_number(21));
        require_error(number_checker, *ctx.get(), ctx->value_make_bool(true));

        auto obj = ctx->object_make(nullptr).to_value();
        require_valid(object_checker, *ctx.get(), obj);
        require_error(array_checker, *ctx.get(), obj);

        auto arr = ctx->object_make_array().to_value();
        require_valid(object_checker, *ctx.get(), arr);
        require_valid(array_checker, *ctx.get(), arr);
        require_error(number_checker, *ctx.get(), arr);
    }
}
