#include <Catch2/catch.hpp>

#ifdef ADV_JSI_JSC
#include <aardvark_jsi/jsc.hpp>
#endif

#ifdef ADV_JSI_QJS
#include <aardvark_jsi/qjs.hpp>
#endif

#include <aardvark_jsi/native.hpp>

using namespace aardvark::jsi;

TEST_CASE("native", "[native]") {
    auto create_context = []() { return Qjs_Context::create(); };

    SECTION("args mapper") {
        auto ctx = create_context();

        auto mapper = ArgsMapper<std::string, int>(
            "name",  // name
            std::make_tuple("arg1", string_mapper),
            std::make_tuple("arg2", int_mapper));

        auto js_args = std::vector{
            ctx->value_make_string(ctx->string_make_from_utf8("test")),
            ctx->value_make_number(2)};
        auto args = mapper.from_js(*ctx.get(), js_args);
        REQUIRE(std::get<0>(args) == "test");
        REQUIRE(std::get<1>(args) == 2);
    }

    SECTION("native function") {
        auto ctx = create_context();
        auto fn = ctx->object_make_function(
            NativeFunction<std::string, void*, bool, int>(
                ctx.get(),       // ctx
                "FunctionName",  // name
                string_mapper,   // res_mapper
                nullptr,         // this_mapper
                {"arg1", bool_mapper},
                {"arg2", int_mapper},  // args_mappers
                [](void* _this, bool arg1, int arg2) {
                    return std::to_string(arg1) + std::to_string(arg2);
                }));
        auto res = fn.call_as_function(
            nullptr, {ctx->value_make_bool(true), ctx->value_make_number(2.0)});
        REQUIRE(res.value().to_string().value().to_utf8() == "12");
    }

    SECTION("native class") {
        auto ctx = create_context();

        class Test {
          public:
            int prop;
        };

        Mapper<std::shared_ptr<Test>>* test_mapper;

        auto method = NativeFunction<int, std::shared_ptr<Test>, int>(
            ctx.get(),            // ctx
            "method",             // name
            int_mapper,           // res_mapper
            test_mapper,          // this_mapper
            {"arg", int_mapper},  // args_mappers
            [](std::shared_ptr<Test> test, int arg) {
                return test->prop + arg;
            });

        auto getter = NativeClassGetter<std::shared_ptr<Test>, int>(
            ctx.get(),    // ctx
            "method",     // name
            test_mapper,  // this_mapper
            int_mapper,   // prop_mapper
            [](std::shared_ptr<Test> test) { return test->prop; });

        auto setter = NativeClassSetter<std::shared_ptr<Test>, int>(
            ctx.get(),    // ctx
            "method",     // name
            test_mapper,  // this_mapper
            int_mapper,   // prop_mapper
            [](std::shared_ptr<Test> test, int val) {
                test->prop = val;
                return true;
            });

        auto def = ClassDefinition();
        def.methods = {{"method", method}};
        def.properties = {{"prop", ClassPropertyDefinition{getter, setter}}};
        auto cls = ctx->class_make(def);
        
        test_mapper = new ObjectsMapper<Test>("Test", cls);

    }
}
