#include "jsc.hpp"

namespace aardvark::jsi {

class JscValue : public PointerData {
  public:
    JscValue(JSGlobalContextRef ctx, bool* ctx_invalid, JSValueRef ref)
        : ctx(ctx), ctx_invalid(ctx_invalid), ref(ref) {
        protect();
    }

    ~JscValue() override {
        if (!*ctx_invalid) JSValueUnprotect(ctx, ref);
    }

    void protect() {
        if (!*ctx_invalid) JSValueProtect(ctx, ref);
    }

    PointerData* copy() override {
        protect();
        return new JscValue(*this);
    }

    bool* ctx_invalid;
    JSGlobalContextRef ctx;
    JSValueRef ref;
};

class JscString : public PointerData {
  public:
    JscString(JSStringRef ref) : ref(ref) {}

    ~JscString() override { JSStringRelease(ref); }

    PointerData* copy() override {
        JSStringRetain(ref);
        return new JscString(*this);
    }

    JSStringRef ref;
};

// Classes are released on context destroy
class JscClass : public PointerData {
  public:
    JscClass(JSClassRef ref) : ref(ref) {}
    PointerData* copy() override { return new JscClass(*this); }
    JSClassRef ref;
};

// TODO just a constructor? why not lazy: message(), location()
struct JsErrorMapper {
    JsError from_js(const Context& ctx, const Value& value) {
        return JsError(
            value,                        // value
            value.to_string().to_utf8(),  // message
            JsErrorLocation{"url", 0, 0}  // TODO location
        );
    }
};

auto js_error_mapper = new JsErrorMapper();

std::shared_ptr<Jsc_Context> Jsc_Context::create() {
    return std::make_shared<Jsc_Context>();
}

Jsc_Context* Jsc_Context::get(JSContextRef ctx) {
    auto global_object = JSContextGetGlobalObject(ctx);
    return static_cast<Jsc_Context*>(JSObjectGetPrivate(global_object));
}

Jsc_Context::Jsc_Context() {
    // Create empty dummy class because default object class does not
    // allocate storage for private data
    auto global_class = JSClassCreate(&kJSClassDefinitionEmpty);
    ctx = JSGlobalContextCreate(global_class);
    auto global_object = JSContextGetGlobalObject(ctx);
    JSObjectSetPrivate(global_object, (void*)this);
    JSClassRelease(global_class);
}

Jsc_Context::~Jsc_Context() {
    ctx_invalid = true;
    JSGlobalContextRelease(ctx);
    for (auto& it : class_definitions) JSClassRelease(it.first);
}

// Helpers

String Jsc_Context::string_from_jsc(JSStringRef ref) {
    return String(this, new JscString(ref));
}

Value Jsc_Context::value_from_jsc(JSValueRef ref) {
    return Value(this, new JscValue(ctx, &ctx_invalid, ref));
}

Object Jsc_Context::object_from_jsc(JSObjectRef ref) {
    return Object(this, new JscValue(ctx, &ctx_invalid, (JSValueRef)ref));
}

Class Jsc_Context::class_from_jsc(JSClassRef ref) {
    return Class(this, new JscClass(ref));
}

JSValueRef Jsc_Context::value_ref(const Value& value) {
    return static_cast<JscValue*>(value.ptr)->ref;
}

JSObjectRef Jsc_Context::object_ref(const Object& object) {
    return (JSObjectRef)static_cast<JscValue*>(object.ptr)->ref;
}

JSStringRef Jsc_Context::string_ref(const String& str) {
    return static_cast<JscString*>(str.ptr)->ref;
}

JSClassRef Jsc_Context::class_ref(const Class& cls) {
    return static_cast<JscClass*>(cls.ptr)->ref;
}

Script Jsc_Context::create_script(
    const std::string& source, const std::string& source_url) {
    // TODO
}

Value Jsc_Context::eval_script(
    const std::string& script, Object* jsi_this,
    const std::string& source_url) {
    auto jsi_script = string_make_from_utf8(script);
    auto js_this = jsi_this == nullptr ? nullptr : object_ref(*jsi_this);
    auto jsi_source_url = string_make_from_utf8(script);
    auto exception = JSValueRef();
    auto jsc_res = JSEvaluateScript(
        ctx, string_ref(jsi_script), js_this, string_ref(jsi_source_url),
        0 /* starting_line_number */, &exception);
    if (exception != nullptr) {
        throw js_error_mapper->from_js(*this, value_from_jsc(exception));
    }
    return value_from_jsc(jsc_res);
}

void Jsc_Context::garbage_collect() { JSGarbageCollect(ctx); }

Object Jsc_Context::get_global_object() {
    return object_from_jsc(JSContextGetGlobalObject(ctx));
}

// String

std::string jsc_string_to_utf8(JSContextRef ctx, JSStringRef jsc_str) {
    auto size = JSStringGetMaximumUTF8CStringSize(jsc_str);
    auto buffer = new char[size];
    JSStringGetUTF8CString(jsc_str, buffer, size);
    auto str = std::string(buffer);
    delete[] buffer;
    return str;
}

String Jsc_Context::string_make_from_utf8(const std::string& str) {
    auto jsc_str = JSStringCreateWithUTF8CString(str.c_str());
    return string_from_jsc(jsc_str);
}

std::string Jsc_Context::string_to_utf8(const String& js_str) {
    return jsc_string_to_utf8(ctx, string_ref(js_str));
}

// Value

Value Jsc_Context::value_make_bool(bool value) {
    return value_from_jsc(JSValueMakeBoolean(ctx, value));
}

Value Jsc_Context::value_make_number(double value) {
    return value_from_jsc(JSValueMakeNumber(ctx, value));
}

Value Jsc_Context::value_make_null() {
    return value_from_jsc(JSValueMakeNull(ctx));
}

Value Jsc_Context::value_make_undefined() {
    return value_from_jsc(JSValueMakeUndefined(ctx));
}

Value Jsc_Context::value_make_string(const String& str) {
    auto jsc_value = JSValueMakeString(ctx, string_ref(str));
    return value_from_jsc(jsc_value);
}

Value Jsc_Context::value_make_object(const Object& object) {
    return value_from_jsc((JSValueRef)object_ref(object));
}

ValueType Jsc_Context::value_get_type(const Value& value) {
    auto type = JSValueGetType(ctx, value_ref(value));
    switch (type) {
        case kJSTypeUndefined:
            return ValueType::undefined;
            break;
        case kJSTypeNull:
            return ValueType::null;
            break;
        case kJSTypeBoolean:
            return ValueType::boolean;
            break;
        case kJSTypeNumber:
            return ValueType::number;
            break;
        case kJSTypeString:
            return ValueType::string;
            break;
        case kJSTypeObject:
            return ValueType::object;
            break;
        case kJSTypeSymbol:
            return ValueType::symbol;
            break;
    }
}

bool Jsc_Context::value_to_bool(const Value& value) {
    return JSValueToBoolean(ctx, value_ref(value));
}

double Jsc_Context::value_to_number(const Value& value) {
    auto exception = JSValueRef();
    if (exception != nullptr) {
        throw js_error_mapper->from_js(*this, value_from_jsc(exception));
    }
    return JSValueToNumber(ctx, value_ref(value), &exception);
}

String Jsc_Context::value_to_string(const Value& value) {
    auto exception = JSValueRef();
    auto jsc_string = JSValueToStringCopy(ctx, value_ref(value), &exception);
    if (exception != nullptr) {
        throw js_error_mapper->from_js(*this, value_from_jsc(exception));
    }
    return string_from_jsc(jsc_string);
}

Object Jsc_Context::value_to_object(const Value& value) {
    auto exception = JSValueRef();
    auto jsc_object = JSValueToObject(ctx, value_ref(value), &exception);
    if (exception != nullptr) {
        throw js_error_mapper->from_js(*this, value_from_jsc(exception));
    }
    return object_from_jsc(jsc_object);
}

bool Jsc_Context::value_strict_equal(const Value& a, const Value& b) {
    return JSValueIsStrictEqual(ctx, value_ref(a), value_ref(b));
}

// Class

std::unordered_map<JSObjectRef, Jsc_Context::ClassInstanceRecord>
    Jsc_Context::class_instances;

ClassDefinition* Jsc_Context::get_class_definition(JSObjectRef object) {
    auto it = class_instances.find(object);
    if (it == class_instances.end()) return nullptr;
    return it->second.definition;
}

void Jsc_Context::finalize_class_instance(JSObjectRef object) {
    auto it = Jsc_Context::class_instances.find(object);
    if (it == Jsc_Context::class_instances.end()) return;
    auto ctx = it->second.ctx;
    auto definition = it->second.definition;
    // TODO
    // if (definition->finalizer) definition->finalizer(object_from_jsc(object));
    Jsc_Context::class_instances.erase(it);
}

void class_finalize(JSObjectRef object) {
    Jsc_Context::finalize_class_instance(object);
}

JSValueRef class_static_value_get(
    JSContextRef ctx, JSObjectRef object, JSStringRef prop_name,
    JSValueRef* exception) {
    auto jsi_ctx = Jsc_Context::get(ctx);
    auto jsi_object = jsi_ctx->object_from_jsc(object);
    auto definition = Jsc_Context::get_class_definition(object);
    auto name = jsc_string_to_utf8(ctx, prop_name);
    auto jsi_ret_val = std::optional<Value>();
    try {
        jsi_ret_val.emplace(definition->properties[name].get(jsi_object));
    } catch (JsError& jsi_ex) {
        *exception = jsi_ctx->value_ref(jsi_ex.value);
        return JSValueMakeUndefined(ctx);
    }
    return jsi_ctx->value_ref(jsi_ret_val.value());
}

bool class_static_value_set(
    JSContextRef ctx, JSObjectRef object, JSStringRef prop_name,
    JSValueRef value, JSValueRef* exception) {
    auto jsi_ctx = Jsc_Context::get(ctx);
    auto jsi_object = jsi_ctx->object_from_jsc(object);
    auto jsi_value = jsi_ctx->value_from_jsc(value);
    auto definition = Jsc_Context::get_class_definition(object);
    auto name = jsc_string_to_utf8(ctx, prop_name);
    auto did_set = false;
    try {
        did_set = definition->properties[name].set(jsi_object, jsi_value);
    } catch (JsError& jsi_ex) {
        *exception = jsi_ctx->value_ref(jsi_ex.value);
        return false;
    }
    return did_set;
}

Class Jsc_Context::class_create(const ClassDefinition& definition) {
    auto jsc_definition = kJSClassDefinitionEmpty;
    jsc_definition.className = definition.name.c_str();
    jsc_definition.finalize = class_finalize;

    // JSC C api has no Object.defineProperty so creating class relies on
    // automatically generated class prototype
    JSStaticValue static_values[definition.properties.size() + 1];
    auto i = 0;
    for (auto& it : definition.properties) {
        static_values[i] = {it.first.c_str(), class_static_value_get,
                            class_static_value_set, kJSPropertyAttributeNone};
        i++;
    }
    static_values[i] = {0, 0, 0};
    jsc_definition.staticValues = static_values;

    auto jsc_class = JSClassCreate(&jsc_definition);
    auto jsi_class = class_from_jsc(jsc_class);

    // Only way to get generated prototype is to create temporary object
    auto instance = JSObjectMake(ctx, jsc_class, nullptr);
    auto proto =
        JSValueToObject(ctx, JSObjectGetPrototype(ctx, instance), nullptr);

    // Add methods to the prototype as usual functions
    if (!definition.methods.empty()) {
        auto jsi_proto = object_from_jsc(proto);
        for (auto& it : definition.methods) {
            auto method_value = object_make_function(it.second).to_value();
            jsi_proto.set_property(it.first, method_value);
        }
    }

    class_definitions[jsc_class] = std::move(definition);
    auto stored_definition = &class_definitions[jsc_class];

    return jsi_class;
}

// Object

Object Jsc_Context::object_make(const Class* js_class) {
    auto exception = JSValueRef();
    auto jsc_object = JSObjectMake(
        ctx, js_class == nullptr ? nullptr : class_ref(*js_class), &exception);
    if (js_class != nullptr) {
        auto definition = &class_definitions.find(class_ref(*js_class))->second;
        Jsc_Context::class_instances.emplace(
            jsc_object, ClassInstanceRecord{this, definition});
    }
    return object_from_jsc(jsc_object);
}

JSValueRef native_function_call_as_function(
    JSContextRef ctx, JSObjectRef function, JSObjectRef this_object,
    size_t arg_count, const JSValueRef args[], JSValueRef* exception) {
    auto jsi_ctx = Jsc_Context::get(ctx);
    auto jsi_function = static_cast<Function*>(JSObjectGetPrivate(function));
    auto jsi_this = this_object == nullptr
                        ? jsi_ctx->value_make_null()
                        : jsi_ctx->value_from_jsc((JSValueRef)this_object);
    auto jsi_args = std::vector<Value>();
    jsi_args.reserve(arg_count);
    for (auto i = 0; i < arg_count; i++) {
        jsi_args.push_back(jsi_ctx->value_from_jsc(args[i]));
    }
    auto jsi_ret_val = std::optional<Value>();
    try {
        jsi_ret_val.emplace((*jsi_function)(jsi_this, jsi_args));
    } catch (JsError& jsi_ex) {
        *exception = jsi_ctx->value_ref(jsi_ex.value);
        return nullptr;
    }
    return jsi_ctx->value_ref(jsi_ret_val.value());
}

void native_function_finalize(JSObjectRef object) {
    delete static_cast<Function*>(JSObjectGetPrivate(object));
}

JSClassRef native_function_create_class() {
    auto definition = kJSClassDefinitionEmpty;
    definition.className = "NativeFunction";
    definition.callAsFunction = native_function_call_as_function;
    definition.finalize = native_function_finalize;
    return JSClassCreate(&definition);
}

auto native_function_class = native_function_create_class();

Object Jsc_Context::object_make_function(const Function& function) {
    auto function_ptr = new Function(function);
    auto jsc_object = JSObjectMake(ctx, native_function_class, function_ptr);
    return object_from_jsc(jsc_object);
}

Object Jsc_Context::object_make_constructor(const Class& js_class) {
    // TODO
}

Object Jsc_Context::object_make_array() {
    auto jsc_array = JSObjectMakeArray(
        ctx,      // ctx
        0,        // args_count
        nullptr,  // args
        nullptr   // exception
    );
    return object_from_jsc(jsc_array);
}

Value Jsc_Context::object_to_value(const Object& object) {
    return value_from_jsc((JSValueRef)object_ref(object));
}

void Jsc_Context::object_set_private_data(const Object& object, void* data) {
    JSObjectSetPrivate(object_ref(object), data);
}

void* Jsc_Context::object_get_private_data(const Object& object) {
    return JSObjectGetPrivate(object_ref(object));
}

Value Jsc_Context::object_get_prototype(const Object& object) {
    auto jsc_proto = JSObjectGetPrototype(ctx, object_ref(object));
    return value_from_jsc(jsc_proto);
}

void Jsc_Context::object_set_prototype(
    const Object& object, const Value& proto) {
    return JSObjectSetPrototype(ctx, object_ref(object), value_ref(proto));
}

std::vector<std::string> Jsc_Context::object_get_property_names(
    const Object& object) {
    auto jsc_names = JSObjectCopyPropertyNames(ctx, object_ref(object));
    auto count = JSPropertyNameArrayGetCount(jsc_names);
    auto names = std::vector<std::string>();
    names.reserve(count);
    for (auto i = 0; i < count; i++) {
        auto name = JSPropertyNameArrayGetNameAtIndex(jsc_names, i);
        names.push_back(jsc_string_to_utf8(ctx, name));
    }
    JSPropertyNameArrayRelease(jsc_names);
    return names;
}

bool Jsc_Context::object_has_property(
    const Object& object, const std::string& name) {
    auto jsc_name = string_make_from_utf8(name);
    return JSObjectHasProperty(ctx, object_ref(object), string_ref(jsc_name));
}

Value Jsc_Context::object_get_property(
    const Object& object, const std::string& name) {
    auto jsc_name = string_make_from_utf8(name);
    auto exception = JSValueRef();
    auto jsc_value = JSObjectGetProperty(
        ctx, object_ref(object), string_ref(jsc_name), &exception);
    if (exception != nullptr) {
        throw js_error_mapper->from_js(*this, value_from_jsc(exception));
    }
    return value_from_jsc(jsc_value);
}

void Jsc_Context::object_set_property(
    const Object& object, const std::string& name, const Value& value) {
    auto jsc_name = string_make_from_utf8(name);
    auto exception = JSValueRef();
    JSObjectSetProperty(
        ctx, object_ref(object), string_ref(jsc_name), value_ref(value),
        kJSPropertyAttributeNone, &exception);
    if (exception != nullptr) {
        throw js_error_mapper->from_js(*this, value_from_jsc(exception));
    }
}

void Jsc_Context::object_delete_property(
    const Object& object, const std::string& name) {
    auto jsc_name = string_make_from_utf8(name);
    auto exception = JSValueRef();
    JSObjectDeleteProperty(
        ctx, object_ref(object), string_ref(jsc_name), &exception);
    if (exception != nullptr) {
        throw js_error_mapper->from_js(*this, value_from_jsc(exception));
    }
}

bool Jsc_Context::object_is_function(const Object& object) {
    return JSObjectIsFunction(ctx, object_ref(object));
}

Value Jsc_Context::object_call_as_function(
    const Object& jsi_object, const Value* jsi_this,
    const std::vector<Value>& jsi_args) {
    auto object = object_ref(jsi_object);
    auto this_object = jsi_this == nullptr
                           ? nullptr
                           : (JSObjectRef)value_ref(*jsi_this);  // TODO
    JSValueRef args[jsi_args.size()];
    for (auto i = 0; i < jsi_args.size(); i++) {
        args[i] = value_ref(jsi_args[i]);
    }
    auto exception = JSValueRef();
    auto jsc_ret_val = JSObjectCallAsFunction(
        ctx, object, this_object, jsi_args.size(), args, &exception);
    if (exception != nullptr) {
        throw js_error_mapper->from_js(*this, value_from_jsc(exception));
    }
    return value_from_jsc(jsc_ret_val);
}

bool Jsc_Context::object_is_constructor(const Object& object) {
    // TODO
}

Value Jsc_Context::object_call_as_constructor(
    const Object& object, const std::vector<Value>& arguments) {
    // TODO
}

bool Jsc_Context::object_is_array(const Object& object) {
    return JSValueIsArray(ctx, object_ref(object));
}

Value Jsc_Context::object_get_property_at_index(
    const Object& object, size_t index) {
    auto exception = JSValueRef();
    auto res =
        JSObjectGetPropertyAtIndex(ctx, object_ref(object), index, &exception);
    if (exception != nullptr) {
        throw js_error_mapper->from_js(*this, value_from_jsc(exception));
    }
    return value_from_jsc(res);
}

void Jsc_Context::object_set_property_at_index(
    const Object& object, size_t index, const Value& value) {
    auto exception = JSValueRef();
    JSObjectSetPropertyAtIndex(
        ctx, object_ref(object), index, value_ref(value), &exception);
    if (exception != nullptr) {
        throw js_error_mapper->from_js(*this, value_from_jsc(exception));
    }
}

}  // namespace aardvark::jsi
