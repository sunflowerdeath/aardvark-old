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

        auto val1 = TestStruct{2, "test"};
        auto res1 = mapper.to_js(ctx_ref, val1);
        REQUIRE(res1.to_object().get_property("num").to_number() == 2);
        REQUIRE(
            res1.to_object().get_property("str").to_string().to_utf8() ==
            "test");

        // invalid type
        auto val2 = ctx->value_make_bool(true);
        auto res2 = mapper.try_from_js(ctx_ref, val2, err_params);
        REQUIRE(res2.has_value() == false);

        // missing prop
        auto val3 = ctx->object_make(nullptr);
        auto res3 = mapper.try_from_js(ctx_ref, val3.to_value(), err_params);
        REQUIRE(res3.has_value() == false);

        // invalid prop type
        auto val4 = ctx->object_make(nullptr);
        val4.set_property("num", ctx->value_make_number(2));
        val4.set_property("str", ctx->value_make_number(2));
        auto res4 = mapper.try_from_js(ctx_ref, val4.to_value(), err_params);
        REQUIRE(res4.has_value() == false);

        // valid
        auto val5 = ctx->object_make(nullptr);
        val5.set_property("num", ctx->value_make_number(2));
        auto str = ctx->value_make_string(ctx->string_make_from_utf8("test"));
        val5.set_property("str", str);
        auto res5 = mapper.try_from_js(ctx_ref, val5.to_value(), err_params);
        REQUIRE(res5.has_value() == true);

    }
}

