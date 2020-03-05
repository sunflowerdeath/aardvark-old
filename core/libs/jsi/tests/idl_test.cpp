#include <Catch2/catch.hpp>

#ifdef ADV_JSI_JSC
#include <aardvark_jsi/jsc.hpp>
#endif

#ifdef ADV_JSI_QJS
#include <aardvark_jsi/qjs.hpp>
#endif

#include "../generated/callback.hpp"
#include "../generated/class.hpp"
#include "../generated/custom.hpp"
#include "../generated/enum.hpp"
#include "../generated/extends.hpp"
#include "../generated/function.hpp"
#include "../generated/optional.hpp"
#include "../generated/proxy.hpp"
#include "../generated/struct.hpp"
#include "../generated/union.hpp"

using namespace aardvark::jsi;

auto err_params = CheckErrorParams{"kind", "name", "target"};

TEST_CASE("idl", "[idl]") {
    auto create_context = []() { return Qjs_Context::create(); };

    SECTION("enum") {
        auto ctx = create_context();
        auto api = test::TestEnumApi(&*ctx.get());

        auto res = ctx->eval("TestEnum.valueA", nullptr, "sourceurl").value();
        REQUIRE(res.to_number().value() == 0);

        // api.TestEnum_mapper.to_js(TestEnum::value_a)
    }

    SECTION("struct") {
        auto ctx = create_context();
        auto api = test::TestStructApi(ctx.get());

        SECTION("to_js") {
            auto val = TestStruct{5, "test"};
            auto res = api.TestStruct_mapper->to_js(*ctx, val);
            auto obj = res.to_object().value();
            REQUIRE(
                obj.get_property("intProp").value().to_number().value() == 5);
            REQUIRE(
                obj.get_property("strProp")
                    .value()
                    .to_string()
                    .value()
                    .to_utf8() == "test");
        }

        SECTION("from_js") {
            auto val = ctx->eval("({intProp: 5, strProp: 'test'})", nullptr, "")
                           .value();
            auto res = api.TestStruct_mapper->from_js(*ctx, val);
            REQUIRE(res.int_prop == 5);
            REQUIRE(res.str_prop == "test");
        }

        SECTION("try_from_js error") {
            auto val = ctx->value_make_bool(true);
            auto res =
                api.TestStruct_mapper->try_from_js(*ctx, val, err_params);
            REQUIRE(res.has_value() == false);
        }

        SECTION("try_from_js missing prop") {
            auto val = ctx->eval("({intProp: 5})", nullptr, "").value();
            auto res =
                api.TestStruct_mapper->try_from_js(*ctx, val, err_params);
            REQUIRE(res.has_value() == false);
        }

        SECTION("try_from_js invalid prop type") {
            auto val =
                ctx->eval("({intProp: 5, strProp: 10})", nullptr, "").value();
            auto res =
                api.TestStruct_mapper->try_from_js(*ctx, val, err_params);
            REQUIRE(res.has_value() == false);
        }
    }

    SECTION("optional") {
        auto ctx = create_context();
        auto api = test::TestOptionalApi(ctx.get());

        SECTION("from_js") {
            auto val = ctx->eval("({intProp: 1, optionalProp: 2})", nullptr, "")
                           .value();
            auto res = api.TestOptionalStruct_mapper->from_js(*ctx, val);
            REQUIRE(res.int_prop == 1);
            REQUIRE(res.optional_prop == 2);
        }

        SECTION("from_js optional") {
            auto val = ctx->eval("({intProp: 1})", nullptr, "")
                           .value();
            auto res = api.TestOptionalStruct_mapper->from_js(*ctx, val);
            REQUIRE(res.int_prop == 1);
            REQUIRE(res.optional_prop == 25); // 25 is the default value
        }
    }

    SECTION("union") {
        auto ctx = create_context();
        auto api = test::TestUnionApi(ctx.get());

        SECTION("to_js") {
            auto val = TestUnion(UnionTypeA{5});
            auto res = api.TestUnion_mapper->to_js(*ctx, val);
            auto obj = res.to_object().value();
            REQUIRE(
                obj.get_property("tag").value().to_string().value().to_utf8() ==
                "a");
            REQUIRE(obj.get_property("prop").value().to_number().value() == 5);
        }

        SECTION("try_from_js") {
            auto obj = ctx->object_make(nullptr);
            obj.set_property(
                "tag", ctx->value_make_string(ctx->string_make_from_utf8("a")));
            obj.set_property("prop", ctx->value_make_number(5));
            auto res = api.TestUnion_mapper->try_from_js(
                *ctx, obj.to_value(), err_params);
            REQUIRE(res.has_value());
            auto a = std::get_if<UnionTypeA>(&res.value());
            REQUIRE(a != nullptr);
            REQUIRE(a->prop == 5);
        }

        SECTION("try_from_js invalid") {
            // not obj
            auto val1 = ctx->value_make_number(5);
            auto res1 =
                api.TestUnion_mapper->try_from_js(*ctx, val1, err_params);
            REQUIRE(res1.has_value() == false);

            // no tag
            auto val2 = ctx->object_make(nullptr);
            auto res2 = api.TestUnion_mapper->try_from_js(
                *ctx, val2.to_value(), err_params);
            REQUIRE(res2.has_value() == false);

            // invalid tag
            auto val3 = ctx->object_make(nullptr);
            val3.set_property(
                "tag", ctx->value_make_string(ctx->string_make_from_utf8("c")));
            auto res3 = api.TestUnion_mapper->try_from_js(
                *ctx, val3.to_value(), err_params);
            REQUIRE(res3.has_value() == false);
        }
    }

    SECTION("class") {
        auto ctx = create_context();
        auto api = test::TestClassApi(ctx.get());

        SECTION("to_js") {
            auto val = std::make_shared<TestClass>(5, true);
            auto js_val = api.TestClass_mapper->to_js(*ctx, val);
            auto js_obj = js_val.to_object().value();
            // getter
            REQUIRE(
                js_obj.get_property("intProp").value().to_number().value() ==
                5);
            // setter
            js_obj.set_property("intProp", ctx->value_make_number(6));
            REQUIRE(val->int_prop == 6);
            // method
            // TODO
        }

        SECTION("constructor") {
            auto js_val =
                ctx->eval("(new TestClass(5, true))", nullptr, "url").value();
            auto js_obj = js_val.to_object().value();
            REQUIRE(
                js_obj.get_property("intProp").value().to_number().value() ==
                5);
        }

        SECTION("from_js") {
            auto val = std::make_shared<TestClass>(5, true);
            auto js_val = api.TestClass_mapper->to_js(*ctx, val);
            auto from_js_val = api.TestClass_mapper->from_js(*ctx, js_val);
            REQUIRE(val == from_js_val);
        }

        SECTION("try_from_js error") {
            auto js_val = ctx->value_make_number(2);
            auto res =
                api.TestClass_mapper->try_from_js(*ctx, js_val, err_params);
            REQUIRE(res.has_value() == false);
        }

        ctx.reset();
    }

    SECTION("extends") {
        auto ctx = create_context();
        auto api = test::TestExtendsApi(ctx.get());

        SECTION("to_js") {
            auto val = std::make_shared<SuperClass>();
            auto js_val = api.SuperClass_mapper->to_js(*ctx, val);
            auto js_obj = js_val.to_object().value();
            ctx->get_global_object().set_property("inst", js_val);
            // method
            auto method_val =
                ctx->eval("inst.baseMethod()", nullptr, "url").value();
            REQUIRE(method_val.to_number().value() == 25);

            // TODO
            /*
            auto res = ctx->eval(
                "inst instanceof SuperClass && inst instanceof BaseClass",
                nullptr,
                "url");
            REQUIRE(res.value().to_bool().value() == true);
            */
        }

        // from_js
        // TODO

        ctx.reset();
    }

    SECTION("function") {
        auto ctx = create_context();
        auto api = test::TestFunctionApi(ctx.get());
        auto res = ctx->eval("testFunction(20, true)", nullptr, "url").value();
        REQUIRE(res.to_string().value().to_utf8() == "21");
    }

    SECTION("callback") {
        auto ctx = create_context();
        auto api = test::TestCallbackApi(ctx.get());

        SECTION("ok") {
            auto js_val =
                ctx->eval("((a, b) => a + b)", nullptr, "url").value();
            auto val = api.TestCallback_mapper->from_js(*ctx, js_val);
            REQUIRE(val(2, 3) == 5);
        }

        SECTION("exception") {
            auto error = std::optional<Error>(std::nullopt);
            api.error_handler = [&error](Error& an_error) {
                error = an_error;
            };
            auto js_val =
                ctx->eval("((a, b) => A + B)", nullptr, "url").value();
            auto val = api.TestCallback_mapper->from_js(*ctx, js_val);
            auto res = val(2, 3);
            REQUIRE(error.has_value());
        }
    }

    SECTION("proxy") {
        auto ctx = create_context();
        auto api = test::TestProxyApi(ctx.get());

        {
            auto val = std::make_shared<ProxyClass>();
            auto js_val = api.ProxyClass_mapper->to_js(*ctx, val);
            auto obj = js_val.to_object().value();

            auto res = obj.get_property("prop");
            REQUIRE(res.value().to_number().value() == 2);

            obj.set_property("prop", ctx->value_make_number(25));
            REQUIRE(val->prop == 2);

            ctx->get_global_object().set_property("inst", js_val);
            auto method_res = ctx->eval("inst.method(2)", nullptr, "url");
            REQUIRE(method_res.value().to_number().value() == 4);
        }

        ctx.reset();
    }

    SECTION("custom") {
        auto ctx = create_context();
        auto api = test::TestCustomApi(ctx.get());

        auto val = CustomType{5,5};
        auto js_val = api.CustomType_mapper->to_js(*ctx, val);
        REQUIRE(js_val.get_type() == ValueType::number);
        REQUIRE(js_val.to_number().value() == 5);

        auto val2 = api.CustomType_mapper->from_js(*ctx, js_val);
        REQUIRE(val2[0] == 5);
    }
}
