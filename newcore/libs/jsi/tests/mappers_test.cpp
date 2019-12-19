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

class TestClass {
  public:
    TestClass(int value) : value(value){};
    int value;
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

    SECTION("func") {
        auto mapper = FunctionMapper<int, int, std::string>(
            int_mapper, int_mapper, string_mapper);

        auto js_func1 =
            ctx->eval_script("func=(a,b)=>a+b.length", nullptr, "sourceurl");
        auto func1 = mapper.from_js(ctx_ref, js_func1);
        REQUIRE(func1(1, "test") == 5);

        auto invalid =
            mapper.try_from_js(ctx_ref, ctx->value_make_bool(true), err_params);
        REQUIRE(invalid.has_value() == false);

        // invalid return value type
        auto js_func2 =
            ctx->eval_script("func=()=>'error'", nullptr, "sourceurl");
        auto func2 = mapper.from_js(ctx_ref, js_func2);
        auto did_throw = false;
        try {
            func2(1, "test");
        } catch (JsError& err) {
            did_throw = true;
        }
        REQUIRE(did_throw);

    }

    SECTION("object") {
        auto def = ClassDefinition();
        def.name = "TestClass";
        auto js_class = ctx->class_create(def);
        auto sptr = std::make_shared<TestClass>(1);

        SECTION("to_js") {
            auto mapper = ObjectsMapper<TestClass>("test", js_class);
            auto object1 = mapper.to_js(ctx_ref, sptr);
            auto object2 = mapper.to_js(ctx_ref, sptr);
            REQUIRE(object1.strict_equal_to(object2));
        }

        SECTION("from_js") {
            auto mapper = ObjectsMapper<TestClass>("test", js_class);
            auto object1 = mapper.to_js(ctx_ref, sptr);
            auto sptr2 = mapper.from_js(ctx_ref, object1);
            REQUIRE(sptr == sptr2);
        }

        // SECTION("remove") {
            // auto index = js::ObjectsIndex<TestClass>(ctx, jsclass);
            // auto object1 = index.get_or_create_js_object(s_ptr);
            // js::ObjectsIndex<TestClass>::remove(object1);
            // auto object2 = index.get_or_create_js_object(s_ptr);
            // REQUIRE(object1 != object2);
        // }
    }
}
