#include "qjs.hpp"

namespace aardvark::jsi {

class QjsValue : public PointerData {
  public:
    QjsValue(JSContext* ctx, const JSValue& value, bool finalizing = false)
        : ctx(ctx), value(value), finalizing(finalizing) {}

    ~QjsValue() override {
        if (!finalizing) JS_FreeValue(ctx, value);
    }

    void protect() {
        if (!finalizing) JS_DupValue(ctx, value);
    }

    PointerData* copy() override {
        protect();
        return new QjsValue(*this);
    }

    JSContext* ctx;
    const JSValue value;
    bool finalizing;
};

class QjsString : public PointerData {
  public:
    // Qjs string copied to std::string
    QjsString(JSContext* ctx, const char* qjs_str) : str(qjs_str) {
        JS_FreeCString(ctx, qjs_str);
    }

    QjsString(const std::string& str) : str(str) {}

    PointerData* copy() override { return new QjsString(*this); }

    std::string str;
};

class QjsClass : public PointerData {
  public:
    QjsClass(JSClassID id) : id(id){};

    PointerData* copy() override { return new QjsClass(*this); }

    JSClassID id;
};

//  Initialization

void native_function_finalizer(JSRuntime* rt, JSValue val) {
    delete static_cast<Function*>(
        JS_GetOpaque(val, Qjs_Context::function_class_id));
}

JSValue native_function_call(
    JSContext* ctx, JSValueConst func_obj, JSValueConst this_val, int argc,
    JSValueConst* argv) {
    auto func = static_cast<Function*>(
        JS_GetOpaque(func_obj, Qjs_Context::function_class_id));
    auto jsi_ctx = Qjs_Context::get(ctx);
    JS_DupValue(ctx, this_val);
    auto jsi_this = jsi_ctx->value_from_qjs(this_val);
    auto jsi_args = std::vector<Value>();
    for (auto i = 0; i < argc; i++) {
        JS_DupValue(ctx, argv[i]);
        jsi_args.push_back(jsi_ctx->value_from_qjs(argv[i]));
    }
    auto res = std::optional<Value>();
    try {
        res.emplace((*func)(jsi_this, jsi_args));
    } catch (JsError& error) {
        auto qjs_ex = jsi_ctx->value_get_qjs(error.value);
        JS_DupValue(ctx, qjs_ex);
        return JS_Throw(ctx, qjs_ex);
    }
    auto qjs_res = jsi_ctx->value_get_qjs(res.value());
    JS_DupValue(ctx, qjs_res);
    return qjs_res;
}

std::unordered_map<void*, Qjs_Context::ClassInstanceRecord>
    Qjs_Context::class_instances;

JSClassID Qjs_Context::function_class_id = 0;

std::shared_ptr<Qjs_Context> Qjs_Context::create() {
    auto ctx = std::make_shared<Qjs_Context>();
    ctx->init();
    return ctx;
}

Qjs_Context* Qjs_Context::get(JSContext* ctx) {
    return static_cast<Qjs_Context*>(JS_GetContextOpaque(ctx));
}

Qjs_Context::Qjs_Context() {
    // do nothing
}

void Qjs_Context::init() {
    rt = JS_NewRuntime();
    ctx = JS_NewContext(rt);
    JS_SetContextOpaque(ctx, (void*)this);

    JS_NewClassID(&Qjs_Context::function_class_id);
    auto function_class_def = JSClassDef{
        "NativeFunction",           // class_name
        native_function_finalizer,  // finalizer
        nullptr,                    // gc_mark
        native_function_call,       // call
        nullptr                     // exotic
    };
    JS_NewClass(rt, Qjs_Context::function_class_id, &function_class_def);

    strict_equal_function.emplace(
        eval_script("eq=(a,b)=>a===b", nullptr, "").to_object());
}

Qjs_Context::~Qjs_Context() {
    strict_equal_function.reset();
    Qjs_Context::function_class_id = 0;
    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);
}

// Helpers

Value Qjs_Context::value_from_qjs(const JSValue& value, bool finalizing) {
    return Value(this, new QjsValue(ctx, value, finalizing));
}

Object Qjs_Context::object_from_qjs(const JSValue& value, bool finalizing) {
    return Object(this, new QjsValue(ctx, value, finalizing));
}

JSValue Qjs_Context::value_get_qjs(const Value& value) {
    return static_cast<QjsValue*>(value.ptr)->value;
}

JSValue Qjs_Context::object_get_qjs(const Object& object) {
    return static_cast<QjsValue*>(object.ptr)->value;
}

std::string Qjs_Context::string_get_str(const String& str) {
    return static_cast<QjsString*>(str.ptr)->str;
}

JSClassID Qjs_Context::class_get_qjs(const Class& cls) {
    return static_cast<QjsClass*>(cls.ptr)->id;
}

JsError Qjs_Context::get_error() {
    return JsError(
        value_from_qjs(JS_GetException(ctx)),  // value
        "",                                    // TODO message
        JsErrorLocation{"url", 0, 0}           // TODO location
    );
}

void Qjs_Context::check_error_value(const JSValue& value) {
    if (JS_IsException(value)) throw get_error();
}

void Qjs_Context::check_error_code(int res) {
    if (res == -1) throw get_error();
}

Script Qjs_Context::create_script(
    const std::string& source, const std::string& source_url) {}

// Global

Value Qjs_Context::eval_script(
    const std::string& script, Object* js_this, const std::string& source_url) {
    auto res = JS_Eval(
        ctx, script.c_str(), script.size() + 1, source_url.c_str(),
        JS_EVAL_TYPE_GLOBAL);
    check_error_value(res);
    return value_from_qjs(res);
}

void Qjs_Context::garbage_collect() { JS_RunGC(rt); }

Object Qjs_Context::get_global_object() {
    return object_from_qjs(JS_GetGlobalObject(ctx));
}

// String
String Qjs_Context::string_make_from_utf8(const std::string& str) {
    return String(this, new QjsString(str));
}

std::string Qjs_Context::string_to_utf8(const String& str) {
    return string_get_str(str);
}

// Value
Value Qjs_Context::value_make_bool(bool value) {
    return value_from_qjs(JS_NewBool(ctx, value));
}

Value Qjs_Context::value_make_number(double value) {
    return value_from_qjs(JS_NewFloat64(ctx, value));
}

Value Qjs_Context::value_make_null() { return value_from_qjs(JS_NULL); }

Value Qjs_Context::value_make_undefined() {
    return value_from_qjs(JS_UNDEFINED);
}

Value Qjs_Context::value_make_string(const String& str) {
    return value_from_qjs(JS_NewString(ctx, string_get_str(str).c_str()));
}

Value Qjs_Context::value_make_object(const Object& object) {
    return Value(this, object.ptr->copy());
}

ValueType Qjs_Context::value_get_type(const Value& value) {
    auto ptr = value_get_qjs(value);
    if (JS_IsBool(ptr)) return ValueType::boolean;
    if (JS_IsNumber(ptr)) return ValueType::number;
    if (JS_IsNull(ptr)) return ValueType::null;
    if (JS_IsUndefined(ptr)) return ValueType::undefined;
    if (JS_IsString(ptr)) return ValueType::string;
    if (JS_IsObject(ptr)) return ValueType::object;
}

bool Qjs_Context::value_to_bool(const Value& value) {
    auto res = JS_ToBool(ctx, value_get_qjs(value));
    if (res == -1) throw get_error();
    return res == 1;
}

double Qjs_Context::value_to_number(const Value& value) {
    double number;
    auto res = JS_ToFloat64(ctx, &number, value_get_qjs(value));
    check_error_code(res);
    return number;
}

String Qjs_Context::value_to_string(const Value& value) {
    auto qjs_str = JS_ToCString(ctx, value_get_qjs(value));
    // TODO: check error
    return String(this, new QjsString(ctx, qjs_str));
}

Object Qjs_Context::value_to_object(const Value& value) {
    // TODO: Qjs has no conversion, probably should check if value is object
    return Object(this, value.ptr->copy());
}

bool Qjs_Context::value_strict_equal(const Value& a, const Value& b) {
    return strict_equal_function.value()
        .call_as_function(nullptr, {a, b})
        .to_bool();
}

void class_finalizer(JSRuntime* rt, JSValue value) {
    auto it = Qjs_Context::class_instances.find(JS_VALUE_GET_PTR(value));
    if (it == Qjs_Context::class_instances.end()) return;
    auto ctx = it->second.ctx;
    auto class_id = it->second.class_id;
    auto finalizer = ctx->class_finalizers[class_id];
    if (finalizer) finalizer(ctx->object_from_qjs(value, true));
    Qjs_Context::class_instances.erase(it);
}

// Class
Class Qjs_Context::class_create(const ClassDefinition& definition) {
    JSClassID class_id = 0;
    JS_NewClassID(&class_id);
    auto class_def = JSClassDef{definition.name.c_str(),  // name
                                class_finalizer,          // finalizer
                                nullptr, nullptr, nullptr};
    JS_NewClass(rt, class_id, &class_def);

    auto proto = JS_NewObject(ctx);
    for (auto& it : definition.properties) {
        auto& [name, prop] = it;
        auto atom = JS_NewAtomLen(ctx, name.c_str(), name.size());
        auto get = object_make_function(
            [getter = prop.get](Value& js_this, std::vector<Value>& args) {
                auto obj = js_this.to_object();
                return getter(obj);
            });
        auto set =
            object_make_function([this, setter = prop.set](
                                     Value& js_this, std::vector<Value>& args) {
                auto obj = js_this.to_object();
                setter(obj, args[0]);
                return value_from_qjs(JS_UNDEFINED);
            });
        JS_DupValue(ctx, object_get_qjs(get));
        JS_DupValue(ctx, object_get_qjs(set));
        JS_DefinePropertyGetSet(
            ctx, proto, atom, object_get_qjs(get), object_get_qjs(set), 0);
        JS_FreeAtom(ctx, atom);
    }

    for (auto it : definition.methods) {
        auto& [name, method] = it;
        auto value = object_make_function(method);
        JS_DupValue(ctx, object_get_qjs(value));
        JS_DefinePropertyValueStr(
            ctx, proto, name.c_str(), object_get_qjs(value),
            JS_PROP_ENUMERABLE);
    }

    JS_SetClassProto(ctx, class_id, proto);
    class_finalizers.emplace(class_id, definition.finalizer);
    return Class(this, new QjsClass(class_id));
}

// Object
Object Qjs_Context::object_make(const Class* cls) {
    auto qjs_object = cls == nullptr
                          ? JS_NewObject(ctx)
                          : JS_NewObjectClass(ctx, class_get_qjs(*cls));
    if (cls != nullptr) {
        Qjs_Context::class_instances.emplace(
            JS_VALUE_GET_PTR(qjs_object),
            ClassInstanceRecord{this, class_get_qjs(*cls)});
    }
    return object_from_qjs(qjs_object);
}

Object Qjs_Context::object_make_function(const Function& function) {
    auto function_ptr = new Function(function);
    auto qjs_object = JS_NewObjectClass(ctx, Qjs_Context::function_class_id);
    JS_SetOpaque(qjs_object, (void*)function_ptr);
    return object_from_qjs(qjs_object);
}

Object Qjs_Context::object_make_constructor(const Class& js_class) {}

Object Qjs_Context::object_make_array() {
    return object_from_qjs(JS_NewArray(ctx));
}

Value Qjs_Context::object_to_value(const Object& object) {
    return Value(this, object.ptr->copy());
}

void Qjs_Context::object_set_private_data(const Object& object, void* data) {
    JS_SetOpaque(object_get_qjs(object), data);
}

void* Qjs_Context::object_get_private_data(const Object& object) {
    auto qjs_value = object_get_qjs(object);
    auto it = class_instances.find(JS_VALUE_GET_PTR(qjs_value));
    if (it == class_instances.end()) return nullptr;
    return JS_GetOpaque(qjs_value, it->second.class_id);
}

Value Qjs_Context::object_get_prototype(const Object& object) {
    auto qjs_proto = JS_GetPrototype(ctx, object_get_qjs(object));
    JS_DupValue(ctx, qjs_proto);
    return value_from_qjs(qjs_proto);
}

void Qjs_Context::object_set_prototype(
    const Object& object, const Value& prototype) {
    JS_SetPrototype(ctx, object_get_qjs(object), value_get_qjs(prototype));
}

std::vector<std::string> Qjs_Context::object_get_property_names(
    const Object& object) {
    JSPropertyEnum* props;
    uint32_t prop_count = 0;
    auto res = JS_GetOwnPropertyNames(
        ctx, &props, &prop_count, object_get_qjs(object), JS_GPN_STRING_MASK);
    check_error_code(res);
    auto jsi_props = std::vector<std::string>();
    for (auto i = 0; i < prop_count; i++) {
        auto prop_name = JS_AtomToCString(ctx, props[i].atom);
        jsi_props.push_back(prop_name);
        JS_FreeAtom(ctx, props[i].atom);
        JS_FreeCString(ctx, prop_name);
    }
    js_free(ctx, props);
    return jsi_props;
}

bool Qjs_Context::object_has_property(
    const Object& object, const std::string& name) {
    auto prop = JS_NewAtomLen(ctx, name.c_str(), name.size());
    auto res = JS_HasProperty(ctx, object_get_qjs(object), prop);
    JS_FreeAtom(ctx, prop);
    check_error_code(res);
    return res == 1;
}

Value Qjs_Context::object_get_property(
    const Object& object, const std::string& name) {
    auto res = JS_GetPropertyStr(ctx, object_get_qjs(object), name.c_str());
    check_error_value(res);
    return value_from_qjs(res);
}

void Qjs_Context::object_delete_property(
    const Object& object, const std::string& name) {
    auto prop = JS_NewAtomLen(ctx, name.c_str(), name.size());
    auto res =
        JS_DeleteProperty(ctx, object_get_qjs(object), prop, JS_PROP_THROW);
    JS_FreeAtom(ctx, prop);
    check_error_code(res);
}

void Qjs_Context::object_set_property(
    const Object& object, const std::string& name, const Value& value) {
    JS_DupValue(ctx, value_get_qjs(value));
    auto res = JS_SetPropertyStr(
        ctx, object_get_qjs(object), name.c_str(), value_get_qjs(value));
    check_error_code(res);
}

bool Qjs_Context::object_is_function(const Object& object) {
    return JS_IsFunction(ctx, object_get_qjs(object)) == 1;
}

Value Qjs_Context::object_call_as_function(
    const Object& object, const Value* jsi_this,
    const std::vector<Value>& jsi_args) {
    auto qjs_object = object_get_qjs(object);
    auto qjs_this = jsi_this == nullptr ? JS_NULL : value_get_qjs(*jsi_this);
    JSValue qjs_args[jsi_args.size()];
    for (auto i = 0; i < jsi_args.size(); i++) {
        qjs_args[i] = value_get_qjs(jsi_args[i]);
    }
    auto qjs_res =
        JS_Call(ctx, qjs_object, qjs_this, jsi_args.size(), qjs_args);
    check_error_value(qjs_res);
    return value_from_qjs(qjs_res);
}

bool Qjs_Context::object_is_constructor(const Object& object) {}

Value Qjs_Context::object_call_as_constructor(
    const Object& object, const std::vector<Value>& arguments) {}

bool Qjs_Context::object_is_array(const Object& object) {
    return JS_IsArray(ctx, object_get_qjs(object));
}

Value Qjs_Context::object_get_property_at_index(
    const Object& object, size_t index) {
    auto res = JS_GetPropertyUint32(ctx, object_get_qjs(object), index);
    check_error_value(res);
    return value_from_qjs(res);
}

void Qjs_Context::object_set_property_at_index(
    const Object& object, size_t index, const Value& value) {
    auto res = JS_SetPropertyUint32(
        ctx, object_get_qjs(object), index, value_get_qjs(value));
    check_error_code(res);
}

}  // namespace aardvark::jsi
