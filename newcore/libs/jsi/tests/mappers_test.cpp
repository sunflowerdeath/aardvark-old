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

enum class TestEnum { one, two, three };

struct TestStruct {
    int num;
    std::string str;
};

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
    auto err_params = CheckErrorParams{"kind", "name", "target"};
    auto ctx = TestType::create();
    ;
    auto& ctx_ref = *ctx.get();

    SECTION("simple") {
        auto js_val = number_mapper->to_js(ctx_ref, 2.0);
        REQUIRE(js_val.to_number() == 2.0);

        auto num_val =
            number_mapper->from_js(ctx_ref, ctx->value_make_number(3));
        REQUIRE(num_val == 3);

        auto invalid_val = ctx->value_make_bool(true);
        auto invalid_res =
            number_mapper->try_from_js(ctx_ref, invalid_val, err_params);
        REQUIRE(invalid_res.has_value() == false);
        auto valid_val = ctx->value_make_number(2);
        auto valid_res =
            number_mapper->try_from_js(ctx_ref, valid_val, err_params);
        REQUIRE(valid_res.has_value() == true);
        REQUIRE(valid_res.value() == 2);
    }

    SECTION("enum") {
        auto mapper = EnumMapper<TestEnum>(int_mapper);

        auto invalid_val = ctx->value_make_bool(true);
        auto invalid_res = mapper.try_from_js(ctx_ref, invalid_val, err_params);
        REQUIRE(invalid_res.has_value() == false);

        auto valid_val = ctx->value_make_number(2);
        auto valid_res = mapper.try_from_js(ctx_ref, valid_val, err_params);
        REQUIRE(valid_res.has_value() == true);
        REQUIRE(valid_res.value() == TestEnum::three);
    }

    SECTION("struct") {
        auto mapper = StructMapper<TestStruct, int, std::string>(
            {"num", &TestStruct::num, int_mapper},
            {"str", &TestStruct::str, string_mapper});

        auto val = TestStruct{2, "test"};
        auto js_val = mapper.to_js(ctx_ref, val);
        REQUIRE(js_val.to_object().get_property("num").to_number() == 2);
        REQUIRE(
            js_val.to_object().get_property("str").to_string().to_utf8() ==
            "test");
    }
}

