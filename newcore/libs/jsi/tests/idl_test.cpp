#include <Catch2/catch.hpp>

#ifdef ADV_JSI_JSC
#include <aardvark_jsi/jsc.hpp>
#endif

#ifdef ADV_JSI_QJS
#include <aardvark_jsi/qjs.hpp>
#endif

#include "../generated/enum.hpp"
#include "../generated/struct.hpp"
#include "../generated/class.hpp"

// #include "../generated/test_enum_api.hpp"
// #include "../generated/test_struct_api.hpp"
// #include "../generated/test_class_api.hpp"

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
        auto& ctx_ref = *ctx.get();
        auto api = test::TestStructApi(ctx.get());
        
        SECTION("to_js") {
            auto val = TestStruct{5, "test"};
            auto res = api.TestStruct_mapper->to_js(ctx_ref, val);
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
            auto res = api.TestStruct_mapper->from_js(ctx_ref, val);
            REQUIRE(res.int_prop == 5);
            REQUIRE(res.str_prop == "test");
        }
        
        SECTION("try_from_js error") {
            auto val = ctx->value_make_bool(true);
            auto res =
                api.TestStruct_mapper->try_from_js(ctx_ref, val, err_params);
            REQUIRE(res.has_value() == false);
        }

        SECTION("try_from_js missing prop") {
            auto val = ctx->eval("({intProp: 5})", nullptr, "").value();
            auto res =
                api.TestStruct_mapper->try_from_js(ctx_ref, val, err_params);
            REQUIRE(res.has_value() == false);
        }

        SECTION("try_from_js invalid prop type") {
            auto val =
                ctx->eval("({intProp: 5, strProp: 10})", nullptr, "").value();
            auto res =
                api.TestStruct_mapper->try_from_js(ctx_ref, val, err_params);
            REQUIRE(res.has_value() == false);
        }
    }

    SECTION("class") {
        auto ctx = create_context();
        auto& ctx_ref = *ctx.get();
        auto api = test::TestClassApi(ctx.get());
    }

    SECTION("function") {
    }

    SECTION("callback") {
    }
}
