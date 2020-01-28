#include <Catch2/catch.hpp>

#ifdef ADV_JSI_JSC
#include <aardvark_jsi/jsc.hpp>
#endif

#ifdef ADV_JSI_QJS
#include <aardvark_jsi/qjs.hpp>
#endif

#include <aardvark_jsi/native.hpp>

#include "../generated/enum.hpp"

using namespace aardvark::jsi;

TEST_CASE("idl", "[idl]") {
    auto create_context = []() { return Qjs_Context::create(); };

    SECTION("enum") {
        auto ctx = create_context();
        auto api = test::EnumTestApi(&*ctx.get());
        
        auto res = ctx->eval("TestEnum.valueA", nullptr, "sourceurl").value();
        REQUIRE(res.to_number().value() == 0);
        
        // api.TestEnum_mapper.to_js(TestEnum::value_a)
    }
}
