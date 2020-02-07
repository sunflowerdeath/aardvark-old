#include <Catch2/catch.hpp>

#ifdef ADV_JSI_JSC
#include <aardvark_jsi/jsc.hpp>
#endif

#ifdef ADV_JSI_QJS
#include <aardvark_jsi/qjs.hpp>
#endif

#include "../generated/callback.hpp"
#include "../generated/class.hpp"
#include "../generated/enum.hpp"
#include "../generated/extends.hpp"
#include "../generated/function.hpp"
#include "../generated/struct.hpp"
#include "../generated/proxy.hpp"

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
        auto js_val = ctx->eval("((a, b) => a + b)", nullptr, "url").value();
        auto val = api.TestCallback_mapper->from_js(*ctx, js_val);
        REQUIRE(val(2, 3) == 5);
    }

    SECTION("proxy") {
        auto ctx = create_context();
        auto api = test::TestProxyApi(ctx.get());

        auto val = std::make_shared<ProxyClass>();
        auto js_val = api.ProxyClass_mapper->to_js(*ctx, val);
        auto obj = js_val.to_object().value();

        auto res = obj.get_property("prop");
        REQUIRE(res.value().to_number().value() == 2);

        obj.set_property("prop", ctx->value_make_number(25));
        REQUIRE(val->prop == 2);
    }
}
