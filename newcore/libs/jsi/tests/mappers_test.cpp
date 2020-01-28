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
    "mappers",
    "[mappers]"
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

    /*
    SECTION("simple") {
        auto js_val = number_mapper->to_js(ctx_ref, 2.0);
        REQUIRE(js_val.to_number() == 2.0);

        SECTION("from_js") {
            auto val = ctx->value_make_number(3.5);
            auto res = number_mapper->from_js(ctx_ref, val);
            REQUIRE(res == 3.5);
        }

        SECTION("try_from_js invalid") {
            auto val = ctx->value_make_bool(true);
            auto res = number_mapper->try_from_js(ctx_ref, val, err_params);
            REQUIRE(res.has_value() == false);
        }

        SECTION("try_from_js valid") {
            auto val = ctx->value_make_number(2);
            auto res = number_mapper->try_from_js(ctx_ref, val, err_params);
            REQUIRE(res.has_value() == true);
            REQUIRE(res.value() == 2);
        }
    }
    */

    SECTION("enum") {
        enum class TestEnum { one, two, three };
        auto mapper = EnumMapper<TestEnum>(int_mapper);

        SECTION("to_js") {
            auto res = mapper.to_js(ctx_ref, TestEnum::three);
            REQUIRE(res.to_number() == 2);
        }

        SECTION("try_from_js invalid") {
            auto val = ctx->value_make_bool(true);
            auto res = mapper.try_from_js(ctx_ref, val, err_params);
            REQUIRE(res.has_value() == false);
        }

        SECTION("try_from_js valid") {
            auto val = ctx->value_make_number(2);
            auto res = mapper.try_from_js(ctx_ref, val, err_params);
            REQUIRE(res.has_value() == true);
            REQUIRE(res.value() == TestEnum::three);
        }
    }

    SECTION("object") {
        class TestClass {
          public:
            TestClass(int value) : value(value){};
            int value;
        };

        auto def = ClassDefinition();
        def.name = "TestClass";
        auto js_class = ctx->class_make(def);
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

        SECTION("finalize") {
            auto mapper = ObjectsMapper<TestClass>("test", js_class);
            auto object1 = mapper.to_js(ctx_ref, sptr);
            ObjectsMapper<TestClass>::finalize(object1);
            auto object2 = mapper.to_js(ctx_ref, sptr);
            REQUIRE(object1.strict_equal_to(object2) == false);
        }
    }
}
