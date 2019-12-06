#include "jsc.hpp"

namespace aardvark::jsi {

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

// Note about "Ownership follows the Create Rule" in JSC C API.
// When pointer is obtained using function that has "copy" or "create" in its
// name, the value should not be protected.
// This applies to strings and classes, but not for values and objects.

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

Jsc_Context::~Jsc_Context() { JSGlobalContextRelease(ctx); }

Script Jsc_Context::create_script(
    const std::string& source, const std::string& source_url) {}

Value Jsc_Context::eval_script(
    const std::string& script, Object* jsi_this,
    const std::string& source_url) {
    auto jsi_script = string_make_from_utf8(script);
    auto js_this =
        jsi_this == nullptr ? nullptr : jsi_this->get_ptr<JSObjectRef>();
    auto jsi_source_url = string_make_from_utf8(script);
    auto exception = JSValueRef();
    auto jsc_res = JSEvaluateScript(
        ctx, jsi_script.get_ptr<JSStringRef>(), js_this,
        jsi_source_url.get_ptr<JSStringRef>(), 0 /* starting_line_number */,
        &exception);
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
    return String(weak_from_this(), (void*)jsc_str, false);
}

std::string Jsc_Context::string_to_utf8(const String& js_str) {
    return jsc_string_to_utf8(ctx, js_str.get_ptr<JSStringRef>());
}

void Jsc_Context::string_protect(void* ptr) {
    JSStringRetain(static_cast<JSStringRef>(ptr));
}

void Jsc_Context::string_unprotect(void* ptr) {
    JSStringRelease(static_cast<JSStringRef>(ptr));
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
    auto jsc_value = JSValueMakeString(ctx, str.get_ptr<JSStringRef>());
    return value_from_jsc(jsc_value);
}

Value Jsc_Context::value_make_object(const Object& object) {
    return value_from_jsc(object.get_ptr<JSObjectRef>());
}

void Jsc_Context::value_protect(void* ptr) {
    JSValueProtect(ctx, static_cast<JSValueRef>(ptr));
}

void Jsc_Context::value_unprotect(void* ptr) {
    JSValueUnprotect(ctx, static_cast<JSValueRef>(ptr));
}

ValueType Jsc_Context::value_get_type(const Value& value) {
    auto type = JSValueGetType(ctx, value.get_ptr<JSValueRef>());
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
    return JSValueToBoolean(ctx, value.get_ptr<JSValueRef>());
}

double Jsc_Context::value_to_number(const Value& value) {
    auto exception = JSValueRef();
    if (exception != nullptr) {
        throw js_error_mapper->from_js(*this, value_from_jsc(exception));
    }
    return JSValueToNumber(ctx, value.get_ptr<JSValueRef>(), &exception);
}

String Jsc_Context::value_to_string(const Value& value) {
    auto exception = JSValueRef();
    auto jsc_string =
        JSValueToStringCopy(ctx, value.get_ptr<JSValueRef>(), &exception);
    if (exception != nullptr) {
        throw js_error_mapper->from_js(*this, value_from_jsc(exception));
    }
    return String(weak_from_this(), jsc_string, false);
}

Object Jsc_Context::value_to_object(const Value& value) {
    auto exception = JSValueRef();
    auto jsc_object =
        JSValueToObject(ctx, value.get_ptr<JSValueRef>(), &exception);
    if (exception != nullptr) {
        throw js_error_mapper->from_js(*this, value_from_jsc(exception));
    }
    return object_from_jsc(jsc_object);
}

bool Jsc_Context::value_strict_equal(const Value& a, const Value& b) {
    return JSValueIsStrictEqual(
        ctx, a.get_ptr<JSValueRef>(), b.get_ptr<JSValueRef>());
}

// Class

ClassDefinition* get_class_definition(JSContextRef ctx, JSObjectRef object) {
    auto proto =
        JSValueToObject(ctx, JSObjectGetPrototype(ctx, object), nullptr);
    auto definition = static_cast<ClassDefinition*>(JSObjectGetPrivate(proto));
    return definition;
}

JSValueRef class_static_value_get(
    JSContextRef ctx, JSObjectRef object, JSStringRef prop_name,
    JSValueRef* exception) {
    auto jsi_ctx = Jsc_Context::get(ctx);
    auto jsi_object = Value(jsi_ctx->weak_from_this(), (void*)object);
    auto definition = get_class_definition(ctx, object);
    auto name = jsc_string_to_utf8(ctx, prop_name);
    auto jsi_ret_val = Value();
    try {
        jsi_ret_val = definition->properties[name].get(jsi_object);
    } catch (JsError& jsi_ex) {
        *exception = jsi_ex.value.get_ptr<JSValueRef>();
        return JSValueMakeUndefined(ctx);
    }
    return jsi_ret_val.get_ptr<JSValueRef>();
}

bool class_static_value_set(
    JSContextRef ctx, JSObjectRef object, JSStringRef prop_name,
    JSValueRef value, JSValueRef* exception) {
    auto jsi_ctx = Jsc_Context::get(ctx);
    auto jsi_object = Value(jsi_ctx->weak_from_this(), (void*)object);
    auto jsi_value = Value(jsi_ctx->weak_from_this(), (void*)value);
    auto definition = get_class_definition(ctx, object);
    auto name = jsc_string_to_utf8(ctx, prop_name);
    auto did_set = false;
    try {
        did_set = definition->properties[name].set(jsi_object, jsi_value);
    } catch (JsError& jsi_ex) {
        *exception = jsi_ex.value.get_ptr<JSValueRef>();
        return false;
    }
    return did_set;
}

Class Jsc_Context::class_create(const ClassDefinition& definition) {
    class_definitions.push_back(std::move(definition));
    // There is no way to know when the class is no longer alive so class
    // definitions are stored in the context until its destruction
    auto stored_definition = &class_definitions.back();

    auto jsc_definition = kJSClassDefinitionEmpty;
    jsc_definition.className = stored_definition->name.c_str();

    // JSC C api has no Object.defineProperty so creating class relies on
    // automatically generated class prototype
    JSStaticValue static_values[stored_definition->properties.size() + 1];
    auto i = 0;
    for (auto& it : stored_definition->properties) {
        static_values[i] = {it.first.c_str(), class_static_value_get,
                            class_static_value_set, kJSPropertyAttributeNone};
        i++;
    }
    static_values[i] = {0, 0, 0};
    jsc_definition.staticValues = static_values;

    auto jsc_class = JSClassCreate(&jsc_definition);
    auto jsi_class = Class(weak_from_this(), (void*)jsc_class, false);

    // Only way to get generated prototype is to create temporary object
    auto instance = JSObjectMake(ctx, jsc_class, nullptr);
    auto proto =
        JSValueToObject(ctx, JSObjectGetPrototype(ctx, instance), nullptr);
    JSObjectSetPrivate(proto, (void*)stored_definition);

    // Add methods to the prototype as usual functions
    if (!stored_definition->methods.empty()) {
        auto jsi_proto = object_from_jsc(proto);
        for (auto& it : stored_definition->methods) {
            auto method_value = object_make_function(it.second).to_value();
            jsi_proto.set_property(it.first, method_value);
        }
    }

    return jsi_class;
}

void Jsc_Context::class_protect(void* ptr) {
    JSClassRetain(static_cast<JSClassRef>(ptr));
}

void Jsc_Context::class_unprotect(void* ptr) {
    JSClassRelease(static_cast<JSClassRef>(ptr));
}

// Object

Object Jsc_Context::object_make(const Class* js_class) {
    auto exception = JSValueRef();
    auto jsc_object = JSObjectMake(
        ctx, js_class == nullptr ? nullptr : js_class->get_ptr<JSClassRef>(),
        &exception);
    return object_from_jsc(jsc_object);
}

JSValueRef native_function_call_as_function(
    JSContextRef ctx, JSObjectRef function, JSObjectRef this_object,
    size_t arg_count, const JSValueRef args[], JSValueRef* exception) {
    auto jsi_ctx = Jsc_Context::get(ctx);
    auto jsi_function = static_cast<Function*>(JSObjectGetPrivate(function));
    auto jsi_this = this_object == nullptr
                        ? jsi_ctx->value_make_null()
                        : Value(jsi_ctx->weak_from_this(), (void*)this_object);
    auto jsi_args = std::vector<Value>();
    jsi_args.reserve(arg_count);
    for (auto i = 0; i < arg_count; i++) {
        jsi_args.emplace_back(jsi_ctx->weak_from_this(), (void*)args[i]);
    }
    auto jsi_ret_val = Value();
    try {
        jsi_ret_val = (*jsi_function)(jsi_this, jsi_args);
    } catch (JsError& jsi_ex) {
        *exception = jsi_ex.value.get_ptr<JSValueRef>();
        return JSValueMakeUndefined(ctx);
    }
    return jsi_ret_val.get_ptr<JSValueRef>();
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

Object Jsc_Context::object_make_constructor(const Class& js_class) {}

void Jsc_Context::object_protect(void* ptr) {
    JSValueProtect(ctx, static_cast<JSValueRef>(ptr));
}

void Jsc_Context::object_unprotect(void* ptr) {
    JSValueUnprotect(ctx, static_cast<JSValueRef>(ptr));
}

Value Jsc_Context::object_to_value(const Object& object) {
    return value_from_jsc(object.get_ptr<JSValueRef>());
}

void Jsc_Context::object_set_private_data(const Object& object, void* data) {
    JSObjectSetPrivate(object.get_ptr<JSObjectRef>(), data);
}

void* Jsc_Context::object_get_private_data(const Object& object) {
    return JSObjectGetPrivate(object.get_ptr<JSObjectRef>());
}

Value Jsc_Context::object_get_prototype(const Object& object) {
    auto jsc_proto = JSObjectGetPrototype(ctx, object.get_ptr<JSObjectRef>());
    return value_from_jsc(jsc_proto);
}

void Jsc_Context::object_set_prototype(
    const Object& object, const Value& prototype) {
    return JSObjectSetPrototype(
        ctx, object.get_ptr<JSObjectRef>(), prototype.get_ptr<JSValueRef>());
}

std::vector<std::string> Jsc_Context::object_get_property_names(
    const Object& object) {
    auto jsc_names =
        JSObjectCopyPropertyNames(ctx, object.get_ptr<JSObjectRef>());
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
        ctx, object.get_ptr<JSObjectRef>(), jsc_name.get_ptr<JSStringRef>());
}

Value Jsc_Context::object_get_property(
    const Object& object, const std::string& name) {
    auto jsc_name = string_make_from_utf8(name);
    auto exception = JSValueRef();
    auto jsc_value = JSObjectGetProperty(
        ctx, object.get_ptr<JSObjectRef>(), jsc_name.get_ptr<JSStringRef>(),
        &exception);
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
        ctx, object.get_ptr<JSObjectRef>(), jsc_name.get_ptr<JSStringRef>(),
        value.get_ptr<JSValueRef>(), kJSPropertyAttributeNone, &exception);
    if (exception != nullptr) {
        throw js_error_mapper->from_js(*this, value_from_jsc(exception));
    }
}

void Jsc_Context::object_delete_property(
    const Object& object, const std::string& name) {
    auto jsc_name = string_make_from_utf8(name);
    auto exception = JSValueRef();
    JSObjectDeleteProperty(
        ctx, object.get_ptr<JSObjectRef>(), jsc_name.get_ptr<JSStringRef>(),
        &exception);
    if (exception != nullptr) {
        throw js_error_mapper->from_js(*this, value_from_jsc(exception));
    }
}

bool Jsc_Context::object_is_function(const Object& object) {
    return JSObjectIsFunction(ctx, object.get_ptr<JSObjectRef>());
}

Value Jsc_Context::object_call_as_function(
    const Object& jsi_object, const Value* jsi_this,
    const std::vector<Value>& jsi_args) {
    auto object = jsi_object.get_ptr<JSObjectRef>();
    auto this_object =
        jsi_this == nullptr ? nullptr : jsi_this->get_ptr<JSObjectRef>();
    JSValueRef args[jsi_args.size()];
    for (auto i = 0; i < jsi_args.size(); i++) {
        args[i] = jsi_args[i].get_ptr<JSValueRef>();
    }
    auto exception = JSValueRef();
    auto jsc_ret_val = JSObjectCallAsFunction(
        ctx, object, this_object, jsi_args.size(), args, &exception);
    if (exception != nullptr) {
        throw js_error_mapper->from_js(*this, value_from_jsc(exception));
    }
    return value_from_jsc(jsc_ret_val);
}

bool Jsc_Context::object_is_constructor(const Object& object) {}

Value Jsc_Context::object_call_as_constructor(
    const Object& object, const std::vector<Value> arguments) {}

bool Jsc_Context::object_is_array(const Object& object) {}

Value Jsc_Context::object_get_value_at_index(
    const Object& object, size_t index) {}

void Jsc_Context::object_set_value_at_index(
    const Object& object, size_t index, const Value& value) {}

// Helpers

String Jsc_Context::string_from_jsc(JSStringRef ptr, bool should_protect) {
    return String(weak_from_this(), (void*)ptr, should_protect);
}

Value Jsc_Context::value_from_jsc(JSValueRef ptr, bool should_protect) {
    return Value(weak_from_this(), (void*)ptr, should_protect);
}

Object Jsc_Context::object_from_jsc(JSObjectRef ptr, bool should_protect) {
    return Object(weak_from_this(), (void*)ptr, should_protect);
}

}  // namespace aardvark::jsi
