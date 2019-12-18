#include <Catch2/catch.hpp>
#include <aardvark_jsi/check.hpp>
#include <aardvark_jsi/jsi.hpp>
#include <aardvark_jsi/mappers.hpp>

#ifdef ADV_JSI_JSC
#include <aardvark_jsi/jsc.hpp>
#endif

#ifdef ADV_JSI_QJS
#include <aardvark_jsi/qjs.hpp>
#endif

using namespace aardvark::jsi;

TEMPLATE_TEST_CASE(
    "mappers", "[mappers]"
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

    SECTION("simple") {
        auto ctx = create_context();
        auto& ctx_ref = *ctx.get();

        auto js_val = number_mapper->to_js(ctx_ref, 2.0);
        REQUIRE(js_val.to_number() == 2.0);

        auto num_val =
            number_mapper->from_js(ctx_ref, ctx->value_make_number(3));
        REQUIRE(num_val == 3);

        auto err_params = CheckErrorParams{"kin", "name", "target"};
        auto invalid_val = ctx->value_make_bool(true);
        auto invalid_res =
            number_mapper->try_from_js(ctx_ref, invalid_val, err_params);
        REQUIRE(invalid_res.has_value() == false);
        auto valid_val = ctx->value_make_number(2);
        auto valid_res =
            number_mapper->try_from_js(ctx_ref, valid_val, err_params);
        REQUIRE(valid_res.has_value() == true);
    }
}

