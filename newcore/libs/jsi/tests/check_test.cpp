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
    const std::string& type, const Context& ctx, const Value& value) {
    auto result = check_type(ctx, value, type, {"kind", "name", "target"});
    REQUIRE(result.has_value());
}

void require_valid(
    const std::string& type, const Context& ctx, const Value& value) {
    auto result = check_type(ctx, value, type, {"kind", "name", "target"});
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
    auto ctx = create_context();

    require_valid("number", *ctx.get(), ctx->value_make_number(21));
    require_error("number", *ctx.get(), ctx->value_make_bool(true));

    auto obj = ctx->object_make(nullptr).to_value();
    require_valid("object", *ctx.get(), obj);
    require_error("array", *ctx.get(), obj);

    auto arr = ctx->object_make_array().to_value();
    require_valid("object", *ctx.get(), arr);
    require_valid("array", *ctx.get(), arr);
    require_error("number", *ctx.get(), arr);
}
