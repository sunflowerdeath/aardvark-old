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

    error_constructor =
        get_global_object().get_property("Error").value().to_object().value();
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

JSValueRef Jsc_Context::value_to_jsc(const Value& value) {
    return static_cast<JscValue*>(value.ptr)->ref;
}

JSObjectRef Jsc_Context::object_to_jsc(const Object& object) {
    return (JSObjectRef) static_cast<JscValue*>(object.ptr)->ref;
}

JSStringRef Jsc_Context::string_to_jsc(const String& str) {
    return static_cast<JscString*>(str.ptr)->ref;
}

JSClassRef Jsc_Context::class_to_jsc(const Class& cls) {
    return static_cast<JscClass*>(cls.ptr)->ref;
}

tl::unexpected<Error> Jsc_Context::error_from_jsc(JSValueRef ref) {
    auto value = value_from_jsc(ref);
    return tl::make_unexpected(Error(this, &value));
}

void Jsc_Context::error_to_jsc(Error& error, JSValueRef* exception) {
    *exception = value_to_jsc(error.value());
}

// Implementation

Result<Value> Jsc_Context::eval(
    const std::string& source,
    Object* this_obj,
    const std::string& source_url) {
    auto jsi_source = string_make_from_utf8(source);
    auto jsc_this = this_obj == nullptr ? nullptr : object_to_jsc(*this_obj);
    auto jsi_source_url = string_make_from_utf8(source_url);
    auto exception = JSValueRef();
    auto jsc_res = JSEvaluateScript(
        ctx,
        string_to_jsc(jsi_source),
        jsc_this,
        string_to_jsc(jsi_source_url),
        0 /* starting_line_number */,
        &exception);
    if (exception != nullptr) return error_from_jsc(exception);
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
    return jsc_string_to_utf8(ctx, string_to_jsc(js_str));
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
    auto jsc_value = JSValueMakeString(ctx, string_to_jsc(str));
    return value_from_jsc(jsc_value);
}

Value Jsc_Context::value_make_object(const Object& object) {
    return value_from_jsc((JSValueRef)object_to_jsc(object));
}

WeakValue Jsc_Context::value_make_weak(const Value& value) {
    // TODO
}

Value Jsc_Context::weak_value_lock(const WeakValue& value) {
    // TODO
}

bool Jsc_Context::value_is_error(const Value& value) {
    return JSValueIsInstanceOfConstructor(
        ctx,
        value_to_jsc(value),
        object_to_jsc(error_constructor.value()),
        nullptr);
}

Value Jsc_Context::value_make_error(const std::string& message) {
    auto jsc_message =
        JSValueMakeString(ctx, string_to_jsc(string_make_from_utf8(message)));
    auto err = JSObjectMakeError(
        ctx,
        1,             // arg_count
        &jsc_message,  // args
        nullptr        // exception
    );
    return value_from_jsc(err);
}

std::optional<ErrorLocation> Jsc_Context::value_get_error_location(
    const Value& value) {
    // TODO
    return std::nullopt;
}

ValueType Jsc_Context::value_get_type(const Value& value) {
    auto type = JSValueGetType(ctx, value_to_jsc(value));
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

Result<bool> Jsc_Context::value_to_bool(const Value& value) {
    return JSValueToBoolean(ctx, value_to_jsc(value));
}

Result<double> Jsc_Context::value_to_number(const Value& value) {
    auto exception = JSValueRef();
    auto res = JSValueToNumber(ctx, value_to_jsc(value), &exception);
    if (exception != nullptr) return error_from_jsc(exception);
    return res;
}

Result<String> Jsc_Context::value_to_string(const Value& value) {
    auto exception = JSValueRef();
    auto jsc_string = JSValueToStringCopy(ctx, value_to_jsc(value), &exception);
    if (exception != nullptr) return error_from_jsc(exception);
    return string_from_jsc(jsc_string);
}

Result<Object> Jsc_Context::value_to_object(const Value& value) {
    auto exception = JSValueRef();
    auto jsc_object = JSValueToObject(ctx, value_to_jsc(value), &exception);
    if (exception != nullptr) return error_from_jsc(exception);
    return object_from_jsc(jsc_object);
}

bool Jsc_Context::value_strict_equal(const Value& a, const Value& b) {
    return JSValueIsStrictEqual(ctx, value_to_jsc(a), value_to_jsc(b));
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
    if (definition->finalizer) {
        definition->finalizer(ctx->object_from_jsc(object));
    }
    Jsc_Context::class_instances.erase(it);
}

void class_finalize(JSObjectRef object) {
    Jsc_Context::finalize_class_instance(object);
}

JSValueRef class_static_value_get(
    JSContextRef ctx,
    JSObjectRef object,
    JSStringRef prop_name,
    JSValueRef* exception) {
    auto jsi_ctx = Jsc_Context::get(ctx);
    auto jsi_object = jsi_ctx->object_from_jsc(object);
    auto definition = Jsc_Context::get_class_definition(object);
    auto name = jsc_string_to_utf8(ctx, prop_name);
    auto jsi_res = definition->properties[name].get(jsi_object);
    if (jsi_res.has_value()) {
        return jsi_ctx->value_to_jsc(jsi_res.value());
    } else {
        jsi_ctx->error_to_jsc(jsi_res.error(), exception);
        return nullptr;
    }
}

bool class_static_value_set(
    JSContextRef ctx,
    JSObjectRef object,
    JSStringRef prop_name,
    JSValueRef value,
    JSValueRef* exception) {
    auto jsi_ctx = Jsc_Context::get(ctx);
    auto jsi_object = jsi_ctx->object_from_jsc(object);
    auto jsi_value = jsi_ctx->value_from_jsc(value);
    auto definition = Jsc_Context::get_class_definition(object);
    auto name = jsc_string_to_utf8(ctx, prop_name);
    auto did_set_res = definition->properties[name].set(jsi_object, jsi_value);
    if (did_set_res.has_value()) {
        return did_set_res.value();
    } else {
        jsi_ctx->error_to_jsc(did_set_res.error(), exception);
        return false;
    }
}

Class Jsc_Context::class_make(const ClassDefinition& definition) {
    auto jsc_definition = kJSClassDefinitionEmpty;
    jsc_definition.className = definition.name.c_str();
    jsc_definition.finalize = class_finalize;

    // JSC C api has no Object.defineProperty so creating class relies on
    // automatically generated class prototype
    JSStaticValue static_values[definition.properties.size() + 1];
    auto i = 0;
    for (auto& it : definition.properties) {
        static_values[i] = {it.first.c_str(),
                            class_static_value_get,
                            class_static_value_set,
                            kJSPropertyAttributeNone};
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
    return jsi_class;
}

// Object

Object Jsc_Context::object_make(const Class* cls) {
    auto exception = JSValueRef();
    auto jsc_object = JSObjectMake(
        ctx, cls == nullptr ? nullptr : class_to_jsc(*cls), &exception);
    if (cls != nullptr) {
        auto definition = &class_definitions.find(class_to_jsc(*cls))->second;
        Jsc_Context::class_instances.emplace(
            jsc_object, ClassInstanceRecord{this, definition});
    }
    return object_from_jsc(jsc_object);
}

JSValueRef native_function_call_as_function(
    JSContextRef ctx,
    JSObjectRef function,
    JSObjectRef this_object,
    size_t arg_count,
    const JSValueRef args[],
    JSValueRef* exception) {
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
    auto jsi_res = (*jsi_function)(jsi_this, jsi_args);
    if (jsi_res.has_value()) {
        return jsi_ctx->value_to_jsc(jsi_res.value());
    } else {
        jsi_ctx->error_to_jsc(jsi_res.error(), exception);
        return nullptr;
    }
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

Object Jsc_Context::object_make_constructor(const Class& cls) {
    // TODO
}

Object Jsc_Context::object_make_constructor2(
    const Class& cls, const Function& function) {
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
    return value_from_jsc((JSValueRef)object_to_jsc(object));
}

void Jsc_Context::object_set_private_data(const Object& object, void* data) {
    JSObjectSetPrivate(object_to_jsc(object), data);
}

void* Jsc_Context::object_get_private_data(const Object& object) {
    return JSObjectGetPrivate(object_to_jsc(object));
}

Result<Value> Jsc_Context::object_get_prototype(const Object& object) {
    auto jsc_proto = JSObjectGetPrototype(ctx, object_to_jsc(object));
    return value_from_jsc(jsc_proto);
}

VoidResult Jsc_Context::object_set_prototype(
    const Object& object, const Value& proto) {
    JSObjectSetPrototype(ctx, object_to_jsc(object), value_to_jsc(proto));
    return VoidResult();
}

std::vector<std::string> Jsc_Context::object_get_property_names(
    const Object& object) {
    auto jsc_names = JSObjectCopyPropertyNames(ctx, object_to_jsc(object));
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
    return JSObjectHasProperty(
        ctx, object_to_jsc(object), string_to_jsc(jsc_name));
}

Result<Value> Jsc_Context::object_get_property(
    const Object& object, const std::string& name) {
    auto jsc_name = string_make_from_utf8(name);
    auto exception = JSValueRef();
    auto jsc_value = JSObjectGetProperty(
        ctx, object_to_jsc(object), string_to_jsc(jsc_name), &exception);
    if (exception != nullptr) return error_from_jsc(exception);
    return value_from_jsc(jsc_value);
}

VoidResult Jsc_Context::object_set_property(
    const Object& object, const std::string& name, const Value& value) {
    auto jsc_name = string_make_from_utf8(name);
    auto exception = JSValueRef();
    JSObjectSetProperty(
        ctx,
        object_to_jsc(object),
        string_to_jsc(jsc_name),
        value_to_jsc(value),
        kJSPropertyAttributeNone,
        &exception);
    if (exception != nullptr) return error_from_jsc(exception);
    return VoidResult();
}

VoidResult Jsc_Context::object_delete_property(
    const Object& object, const std::string& name) {
    auto jsc_name = string_make_from_utf8(name);
    auto exception = JSValueRef();
    JSObjectDeleteProperty(
        ctx, object_to_jsc(object), string_to_jsc(jsc_name), &exception);
    if (exception != nullptr) return error_from_jsc(exception);
    return VoidResult();
}

bool Jsc_Context::object_is_function(const Object& object) {
    return JSObjectIsFunction(ctx, object_to_jsc(object));
}

Result<Value> Jsc_Context::object_call_as_function(
    const Object& object,
    const Value* this_val,
    const std::vector<Value>& args) {
    auto jsc_obj = object_to_jsc(object);
    auto jsc_this = this_val == nullptr
                        ? nullptr
                        : (JSObjectRef)value_to_jsc(*this_val);  // TODO
    JSValueRef jsc_args[args.size()];
    for (auto i = 0; i < args.size(); i++) {
        jsc_args[i] = value_to_jsc(args[i]);
    }
    auto exception = JSValueRef();
    auto jsc_res = JSObjectCallAsFunction(
        ctx, jsc_obj, jsc_this, args.size(), jsc_args, &exception);
    if (exception != nullptr) return error_from_jsc(exception);
    return value_from_jsc(jsc_res);
}

bool Jsc_Context::object_is_constructor(const Object& object) {
    // TODO
}

Result<Object> Jsc_Context::object_call_as_constructor(
    const Object& object, const std::vector<Value>& args) {
    auto jsc_obj = object_to_jsc(object);
    JSValueRef jsc_args[args.size()];
    for (auto i = 0; i < args.size(); i++) {
        jsc_args[i] = value_to_jsc(args[i]);
    }
    auto exception = JSValueRef();
    auto jsc_res = JSObjectCallAsConstructor(
        ctx, jsc_obj, args.size(), jsc_args, &exception);
    if (exception != nullptr) return error_from_jsc(exception);
    return object_from_jsc(jsc_res);
}

bool Jsc_Context::object_is_array(const Object& object) {
    return JSValueIsArray(ctx, object_to_jsc(object));
}

Result<Value> Jsc_Context::object_get_property_at_index(
    const Object& object, size_t index) {
    auto exception = JSValueRef();
    auto res = JSObjectGetPropertyAtIndex(
        ctx, object_to_jsc(object), index, &exception);
    if (exception != nullptr) return error_from_jsc(exception);
    return value_from_jsc(res);
}

VoidResult Jsc_Context::object_set_property_at_index(
    const Object& object, size_t index, const Value& value) {
    auto exception = JSValueRef();
    JSObjectSetPropertyAtIndex(
        ctx, object_to_jsc(object), index, value_to_jsc(value), &exception);
    if (exception != nullptr) return error_from_jsc(exception);
    return VoidResult();
}

}  // namespace aardvark::jsi
