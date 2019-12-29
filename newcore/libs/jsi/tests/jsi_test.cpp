#include <Catch2/catch.hpp>

#ifdef ADV_JSI_JSC
#include <aardvark_jsi/jsc.hpp>
#endif

#ifdef ADV_JSI_QJS
#include <aardvark_jsi/qjs.hpp>
#endif

using namespace aardvark::jsi;

TEMPLATE_TEST_CASE(
    "context", "[context]"
#ifdef ADV_JSI_QJS
    ,Qjs_Context
#endif
#ifdef ADV_JSI_JSC
    ,Jsc_Context
#endif
) {
    auto create_context = []() { return TestType::create(); };

    SECTION("eval") {
        auto ctx = create_context();

        auto res = ctx->eval_script("2 + 3", nullptr, "source_url");
        REQUIRE(res.value().to_number().value() == 5);
    }

#ifdef ADV_JSI_JSC
    SECTION("eval this") {
        auto ctx = create_context();

        // Qjs doest not supports this
        if (typeid(ctx.get()) != typeid(Jsc_Context*)) return;

        auto this_obj = ctx->object_make(nullptr);
        this_obj.set_property("a", ctx->value_make_number(2));
        auto res = ctx->eval_script("this.a + 3", &this_obj, "source_url");
        REQUIRE(res.value().to_number().value() == 5);
    }
#endif

    SECTION("eval exception") {
        auto ctx = create_context();

        auto res1 = ctx->eval_script("a/b/", nullptr, "source_url");
        REQUIRE(res1.has_value() == false);

        auto res2 = ctx->eval_script("2+2", nullptr, "source_url");
        REQUIRE(res2.has_value() == true);

        auto res3 = ctx->eval_script("a/b/", nullptr, "source_url");
        REQUIRE(res3.has_value() == false);
    }

    SECTION("string") {
        auto ctx = create_context();

        auto str = ctx->string_make_from_utf8("test");
        REQUIRE(str.to_utf8() == "test");
    }

    SECTION("value") {
        auto ctx = create_context();

        auto null_val = ctx->value_make_null();
        REQUIRE(null_val.get_type() == ValueType::null);

        auto undef_val = ctx->value_make_undefined();
        REQUIRE(undef_val.get_type() == ValueType::undefined);

        auto num_val = ctx->value_make_number(1.5);
        REQUIRE(num_val.to_number().value() == 1.5);
        REQUIRE(num_val.get_type() == ValueType::number);

        auto bool_val = ctx->value_make_bool(true);
        REQUIRE(bool_val.to_bool().value());
        REQUIRE(bool_val.get_type() == ValueType::boolean);

        auto str = ctx->string_make_from_utf8("test");
        auto str_val = ctx->value_make_string(str);
        REQUIRE(str_val.get_type() == ValueType::string);
        REQUIRE(str_val.to_string().value().to_utf8() == "test");
    }

    SECTION("strictequal") {
        auto ctx = create_context();

        auto a = ctx->value_make_number(1);
        auto b = ctx->value_make_number(1);
        auto c = ctx->value_make_number(2);
        REQUIRE(a.strict_equal_to(a) == true);
        REQUIRE(a.strict_equal_to(b) == true);
        REQUIRE(a.strict_equal_to(c) == false);

        auto d = ctx->object_make(nullptr).to_value();
        auto e = ctx->object_make(nullptr).to_value();
        REQUIRE(d.strict_equal_to(d) == true);
        REQUIRE(d.strict_equal_to(e) == false);
    }

    SECTION("error") {
        auto ctx = create_context();

        auto not_err = ctx->value_make_number(1);
        auto err = ctx->value_make_error("MESSAGE");
        REQUIRE(not_err.is_error() == false);
        REQUIRE(err.is_error() == true);
        REQUIRE(
            err.to_object()
                .value()
                .get_property("message")
                .value()
                .to_string()
                .value()
                .to_utf8() == "MESSAGE");
    }

    SECTION("object props") {
        auto ctx = create_context();

        auto obj = ctx->object_make(nullptr);
        auto val = ctx->value_make_number(1);
        obj.set_property("a", val);
        REQUIRE(obj.has_property("a") == true);
        REQUIRE(obj.has_property("b") == false);
        auto prop_names = obj.get_property_names();
        REQUIRE(prop_names.size() == 1);
        REQUIRE(prop_names[0] == "a");
        REQUIRE(obj.get_property("a").value().to_number() == 1);
        obj.delete_property("a");
        REQUIRE(obj.has_property("a") == false);
    }

    SECTION("object proto") {
        auto ctx = create_context();

        auto obj = ctx->object_make(nullptr);
        auto proto = ctx->object_make(nullptr);
        auto proto_val = ctx->value_make_object(proto);

        obj.set_prototype(proto_val);
        auto curr_proto = obj.get_prototype().value();
        REQUIRE(curr_proto.get_type() == ValueType::object);
        REQUIRE(curr_proto.strict_equal_to(proto_val));
    }

    SECTION("function") {
        auto ctx = create_context();

        auto is_called = false;
        auto out_this = std::optional<Value>();
        auto out_args = std::vector<Value>();
        auto in_ret_val = ctx->value_make_number(5);
        auto func = [&](const Value& xthis, const std::vector<Value>& args) {
            is_called = true;
            out_this = xthis;
            out_args = args;
            return in_ret_val;
        };
        auto obj = ctx->object_make_function(func);

        REQUIRE(obj.is_function() == true);
        auto in_this = ctx->value_make_object(ctx->object_make(nullptr));
        auto in_arg = ctx->value_make_number(2);
        auto out_ret_val = obj.call_as_function(&in_this, {in_arg}).value();
        REQUIRE(is_called);
        REQUIRE(out_ret_val.to_number().value() == 5);
        REQUIRE(out_this->strict_equal_to(in_this));
        REQUIRE(out_args.size() == 1);
        REQUIRE(out_args[0].to_number().value() == 2);
    }

    SECTION("function exception") {
        auto ctx = create_context();

        auto func = [&](const Value& xthis, const std::vector<Value>& args) {
            return ctx->eval_script("a/b", nullptr, "sourceurl");
        };
        auto obj = ctx->object_make_function(func);

        auto res = obj.call_as_function(nullptr, {});
        REQUIRE(res.has_value() == false);
    }

    SECTION("private") {
        auto ctx = create_context();

        auto definition = ClassDefinition();
        definition.name = "TestClass";
        auto cls = ctx->class_make(definition);
        auto instance = ctx->object_make(&cls);
        auto data = 25;
        instance.set_private_data(static_cast<void*>(&data));
        auto get_data = instance.template get_private_data<int>(); // WTF
        REQUIRE(get_data == 25);
    }

    SECTION("class") {
        auto finalizer_called = false;

        {
            auto ctx = create_context();

            auto prop_value = 1;
            auto get = [&](Object& object) {
                return ctx->value_make_number(prop_value);
            };
            auto set = [&](Object& object, Value& value) {
                prop_value = value.to_number().value();
                return true;
            };
            auto method = [&](Value js_this, std::vector<Value>& args) {
                prop_value = args[0].to_number().value();
                return args[0];
            };
            auto finalizer = [&](const Object& object) {
                finalizer_called = true;
            };

            auto definition = ClassDefinition();
            definition.name = "TestClass";
            definition.methods = {{"method", method}};
            definition.properties = {
                {"prop", ClassPropertyDefinition{get, set}}};
            definition.finalizer = finalizer;

            auto cls = ctx->class_make(definition);
            auto instance = ctx->object_make(&cls);

            REQUIRE(instance.has_property("prop"));

            auto getter_res = instance.get_property("prop").value();
            REQUIRE(getter_res.to_number().value() == 1);

            instance.set_property("prop", ctx->value_make_number(2));
            REQUIRE(prop_value == 2);

            ctx->get_global_object().set_property(
                "instance", instance.to_value());
            auto ret_val =
                ctx->eval_script("instance.method(3)", nullptr, "sourceurl")
                    .value();
            REQUIRE(prop_value == 3);
            REQUIRE(ret_val.to_number().value() == 3);
        }

        REQUIRE(finalizer_called == true);
    }

    SECTION("class exception") {
        auto ctx = create_context();

        auto get = [&](Object& object) {
            return ctx->eval_script("a/b", nullptr, "sourceurl");
        };
        auto set = [&](Object object, Value& value) -> Result<bool> {
            auto res = ctx->eval_script("a/b", nullptr, "sourceurl");
            if (!res.has_value()) return tl::make_unexpected(res.error());
            return true;
        };
        auto method = [&](Value& js_this, std::vector<Value>& args) {
            return ctx->eval_script("a/b", nullptr, "sourceurl");
        };

        auto definition = new ClassDefinition();
        definition->name = "TestClass";
        definition->methods = {{"method", method}};
        definition->properties = {{"prop", ClassPropertyDefinition{get, set}}};
        auto cls = ctx->class_make(*definition);
        delete definition;  // Ensure that definition is not used
        auto instance = ctx->object_make(&cls);

        auto getter_res = instance.get_property("prop");
        REQUIRE(getter_res.has_value() == false);

        auto setter_res =
            instance.set_property("prop", ctx->value_make_number(1));
        REQUIRE(setter_res.has_value() == false);

        auto method_res =
            ctx->eval_script("this.method()", &instance, "sourceurl");
        REQUIRE(method_res.has_value() == false);
    }

    SECTION("constructor") {
        auto ctx = create_context();

        ctx->eval_script(
            "class A{constructor(a) { this.a = a }};", nullptr, "sourceurl");
        auto ctor = ctx->eval_script("A", nullptr, "sourceurl")
                        .value()
                        .to_object()
                        .value();
        REQUIRE(ctor.is_constructor() == true);
        auto args = std::vector<Value>{ctx->value_make_number(5)};
        auto res = ctor.call_as_constructor(args).value();
        REQUIRE(res.get_property("a").value().to_number().value() == 5);

        auto val = ctx->value_make_number(55);
        auto get = [&](Object& object) { return val; };
        auto def = new ClassDefinition();
        def->name = "TestClass";
        def->properties = {{"prop", ClassPropertyDefinition{get, nullptr}}};
        auto cls = ctx->class_make(*def);
        auto native_ctor = ctx->object_make_constructor(cls);
        REQUIRE(native_ctor.is_constructor());
        auto inst = native_ctor.call_as_constructor({}).value();
        REQUIRE(inst.has_property("prop"));
    }

    SECTION("array") {
        auto ctx = create_context();

        auto not_arr = ctx->object_make(nullptr);
        REQUIRE(not_arr.is_array() == false);

        auto arr = ctx->object_make_array();
        REQUIRE(arr.is_array() == true);

        arr.set_property_at_index(0, ctx->value_make_number(1));
        arr.set_property_at_index(1, ctx->value_make_number(2));
        REQUIRE(arr.get_property("length").value().to_number().value() == 2);
        REQUIRE(arr.get_property_at_index(1).value().to_number().value() == 2);
        REQUIRE(arr.get_property("1").value().to_number().value() == 2);
    }
}
