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

    SECTION("struct") {
        struct TestStruct {
            int num;
            std::string str;
        };

        auto mapper = StructMapper<TestStruct, int, std::string>(
            {"num", &TestStruct::num, int_mapper},
            {"str", &TestStruct::str, string_mapper});

        SECTION("to_js") {
            auto val = TestStruct{2, "test"};
            auto res = mapper.to_js(ctx_ref, val);
            REQUIRE(
                res.to_object()
                    .value()
                    .get_property("num")
                    .value()
                    .to_number()
                    .value() == 2);
            REQUIRE(
                res.to_object()
                    .value()
                    .get_property("str")
                    .value()
                    .to_string()
                    .value()
                    .to_utf8() == "test");
        }

        SECTION("try_from_js invalid") {
            auto val = ctx->value_make_bool(true);
            auto res = mapper.try_from_js(ctx_ref, val, err_params);
            REQUIRE(res.has_value() == false);
        }

        SECTION("try_from_js missing prop") {
            auto val = ctx->object_make(nullptr);
            auto res = mapper.try_from_js(ctx_ref, val.to_value(), err_params);
            REQUIRE(res.has_value() == false);
        }

        SECTION("try_from_js invalid prop type") {
            auto val = ctx->object_make(nullptr);
            val.set_property("num", ctx->value_make_number(2));
            val.set_property("str", ctx->value_make_number(2));
            auto res = mapper.try_from_js(ctx_ref, val.to_value(), err_params);
            REQUIRE(res.has_value() == false);
        }

        SECTION("try_from_js valid") {
            auto val = ctx->object_make(nullptr);
            val.set_property("num", ctx->value_make_number(2));
            auto str =
                ctx->value_make_string(ctx->string_make_from_utf8("test"));
            val.set_property("str", str);
            auto res = mapper.try_from_js(ctx_ref, val.to_value(), err_params);
            REQUIRE(res.has_value() == true);
        }
    }

    SECTION("func") {
        auto mapper = FunctionMapper<int, int, std::string>(
            int_mapper, int_mapper, string_mapper);

        SECTION("valid") {
            auto val =
                ctx->eval("(a,b)=>a+b.length", nullptr, "sourceurl").value();
            auto res = mapper.from_js(ctx_ref, val);
            REQUIRE(res(1, "test") == 5);
        }

        SECTION("invalid") {
            auto val = ctx->value_make_bool(true);
            auto res = mapper.try_from_js(ctx_ref, val, err_params);
            REQUIRE(res.has_value() == false);
        }

        SECTION("invalid return type") {
            auto called_handler = false;
            auto mapper = FunctionMapper<int>(
                int_mapper, [&](const Value& error) { called_handler = true; });
            auto val = ctx->eval("()=>'error'", nullptr, "sourceurl").value();
            auto fn = mapper.from_js(ctx_ref, val);
            auto res = fn();
            REQUIRE(called_handler);
            REQUIRE(res == 0);
        }

        struct NotDefaultConstructible {
          public:
            NotDefaultConstructible(int a) : a(a){};
            int a;
        };

        auto ndc_mapper = SimpleMapper<NotDefaultConstructible>(
            [](Context& ctx, const NotDefaultConstructible& val) {
                return ctx.value_make_number(val.a);
            },
            [](Context& ctx, const Value& val) {
                return NotDefaultConstructible(
                    static_cast<int>(val.to_number().value()));
            },
            &number_checker);

        SECTION("fallback") {
            auto called_handler = false;
            auto mapper = FunctionMapper<NotDefaultConstructible>(
                &ndc_mapper,  // res_mapper
                [&](const Value& error) {
                    called_handler = true;
                },                                            // error_handler
                []() { return NotDefaultConstructible(-1); }  // fallback
            );
            auto val = ctx->eval("()=>a/a", nullptr, "sourceurl").value();
            auto fn = mapper.from_js(ctx_ref, val);
            auto res = fn();
            REQUIRE(called_handler);
            REQUIRE(res.a == -1);
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
