#include "qjs.hpp"

#include <iostream>

namespace aardvark::jsi {

class QjsValue : public PointerData {
  public:
    QjsValue(JSContext* ctx, const JSValue& value) : ctx(ctx), value(value) {
        protect();
    }

    ~QjsValue() override {
        JS_FreeValue(ctx, value);
    }

    void protect() {
        JS_DupValue(ctx, value);
    }

    PointerData* copy() override {
        protect();
        return new QjsValue(*this);
    }

    JSContext* ctx;
    const JSValue value;
};

std::shared_ptr<Qjs_Context> Qjs_Context::create() {
    auto ctx = std::make_shared<Qjs_Context>();
    ctx->init();
    return ctx;
}

Qjs_Context::Qjs_Context() {
    // do nothing
}

void Qjs_Context::init() {
    rt = JS_NewRuntime();
    ctx = JS_NewContext(rt);
}

Qjs_Context::~Qjs_Context() {
    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);
}

Value Qjs_Context::value_from_qjs(const JSValue& value) {
    return Value(this, new QjsValue(ctx, value));
}

Object Qjs_Context::object_from_qjs(const JSValue& value) {
    return Object(this, new QjsValue(ctx, value));
}

JSValue Qjs_Context::value_get(const Value& value) {
    return static_cast<QjsValue*>(value.ptr)->value;
}

Script Qjs_Context::create_script(
    const std::string& source, const std::string& source_url) {}

Value Qjs_Context::eval_script(
    const std::string& script, Object* js_this, const std::string& source_url) {

    auto res = JS_Eval(
        ctx, script.c_str(), script.size() + 1, source_url.c_str(),
        JS_EVAL_TYPE_GLOBAL);

    auto is_error = JS_IsException(res);
    if (is_error) {
        JS_FreeValue(ctx, res);
        auto jsi_ex = value_from_qjs(JS_GetException(ctx));
        throw JsError(
            jsi_ex,                       // value
            "",                           // TODO message
            JsErrorLocation{"url", 0, 0}  // TODO location
        );
    }
    return value_from_qjs(res);
}

void Qjs_Context::garbage_collect() {}

Object Qjs_Context::get_global_object() {
    return object_from_qjs(JS_GetGlobalObject(ctx));
}

// String
String Qjs_Context::string_make_from_utf8(const std::string& str) {}
std::string Qjs_Context::string_to_utf8(const String&) {}

// Value
Value Qjs_Context::value_make_bool(bool value) {}

Value Qjs_Context::value_make_number(double value) {
}

Value Qjs_Context::value_make_null() {}
Value Qjs_Context::value_make_undefined() {}
Value Qjs_Context::value_make_string(const String& str) {}
Value Qjs_Context::value_make_object(const Object& object) {}

ValueType Qjs_Context::value_get_type(const Value& value) {
    auto ptr = value_get(value);
    if (JS_IsBool(ptr)) return ValueType::boolean;
    if (JS_IsNumber(ptr)) return ValueType::number;
    if (JS_IsNull(ptr)) return ValueType::null;
    if (JS_IsUndefined(ptr)) return ValueType::undefined;
    if (JS_IsString(ptr)) return ValueType::string;
    if (JS_IsObject(ptr)) return ValueType::object;
}

bool Qjs_Context::value_to_bool(const Value& value) {
    auto res = JS_ToBool(ctx, value_get(value));
    if (res == -1) {
        // check error
    }
    return res == 1;
}

double Qjs_Context::value_to_number(const Value& value) {
    double number;
    auto res = JS_ToFloat64(ctx, &number, value_get(value));
    if (res == -1) {
        // TODO check error
    }
    return number;
}

String Qjs_Context::value_to_string(const Value& value) {}
Object Qjs_Context::value_to_object(const Value& value) {}

bool Qjs_Context::value_strict_equal(const Value& a, const Value& b) {}

// Class
Class Qjs_Context::class_create(const ClassDefinition& definition) {}

// Object
Object Qjs_Context::object_make(const Class* js_class) {}
Object Qjs_Context::object_make_function(const Function& function) {}
Object Qjs_Context::object_make_constructor(const Class& js_class) {}
Object Qjs_Context::object_make_array() {}

Value Qjs_Context::object_to_value(const Object& object) {}

void Qjs_Context::object_set_private_data(const Object& object, void* data) {}
void* Qjs_Context::object_get_private_data(const Object& object) {}

Value Qjs_Context::object_get_prototype(const Object& object) {}
void Qjs_Context::object_set_prototype(
    const Object& object, const Value& prototype) {}

std::vector<std::string> Qjs_Context::object_get_property_names(
    const Object& object) {}
bool Qjs_Context::object_has_property(
    const Object& object, const std::string& name) {}
Value Qjs_Context::object_get_property(
    const Object& object, const std::string& name) {}
void Qjs_Context::object_delete_property(
    const Object& object, const std::string& name) {}
void Qjs_Context::object_set_property(
    const Object& object, const std::string& name, const Value& value) {}

bool Qjs_Context::object_is_function(const Object& object) {}
Value Qjs_Context::object_call_as_function(
    const Object& object, const Value* jsi_this,
    const std::vector<Value>& jsi_args) {}

bool Qjs_Context::object_is_constructor(const Object& object) {}
Value Qjs_Context::object_call_as_constructor(
    const Object& object, const std::vector<Value>& arguments) {}

bool Qjs_Context::object_is_array(const Object& object) {}
Value Qjs_Context::object_get_property_at_index(
    const Object& object, size_t index) {}
void Qjs_Context::object_set_property_at_index(
    const Object& object, size_t index, const Value& value) {}

}  // namespace aardvark::jsi
