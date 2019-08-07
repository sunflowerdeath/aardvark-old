#include "base_types_bindings.hpp"

namespace aardvark::js {

// Helpers

template <class T, T (*from_js)(JSContextRef, JSValueRef)>
void map_prop_from_js(JSContextRef ctx, JSObjectRef object,
                      const char* prop_name, T* out) {
    auto prop_name_str = JSStringCreateWithUTF8CString(prop_name);
    if (JSObjectHasProperty(ctx, object, prop_name_str)) {
        auto prop_value =
            JSObjectGetProperty(ctx, object, prop_name_str, nullptr);
        *out = from_js(ctx, prop_value);
    }
}

template <class T, JSValueRef (*to_js)(JSContextRef, const T&)>
void map_prop_to_js(JSContextRef ctx, JSObjectRef object, const char* prop_name,
                    const T& value) {
    JSObjectSetProperty(ctx, object, JSStringCreateWithUTF8CString(prop_name),
                        to_js(ctx, value), kJSPropertyAttributeNone, nullptr);
}

// Value

Value value_from_js(JSContextRef ctx, JSValueRef js_value) {
    auto object = JSValueToObject(ctx, js_value, nullptr);
    auto type_prop = JSObjectGetProperty(
        ctx, object, JSStringCreateWithUTF8CString("type"), nullptr);
    auto type_str = JSValueToStringCopy(ctx, type_prop, nullptr);
    Value::ValueType type;
    if (JSStringIsEqualToUTF8CString(type_str, "abs")) {
        type = Value::ValueType::abs;
    } else if (JSStringIsEqualToUTF8CString(type_str, "rel")) {
        type = Value::ValueType::rel;
    } else {
        type = Value::ValueType::none;
    }
    float value = 0;
    if (type != Value::ValueType::none) {
        auto value_prop = JSObjectGetProperty(
            ctx, object, JSStringCreateWithUTF8CString("value"), nullptr);
        value = JSValueToNumber(ctx, value_prop, nullptr);
    }
    return Value(type, value);
}

JSValueRef value_to_js(JSContextRef ctx, const Value& value) {
    auto object = JSObjectMake(ctx, nullptr, nullptr);
    JSStringRef type;
    if (value.type == Value::ValueType::abs) {
        type = JSStringCreateWithUTF8CString("abs");
    } else if (value.type == Value::ValueType::rel) {
        type = JSStringCreateWithUTF8CString("rel");
    } else {
        type = JSStringCreateWithUTF8CString("none");
    }
    JSObjectSetProperty(ctx, object, JSStringCreateWithUTF8CString("type"),
                        JSValueMakeString(ctx, type), kJSPropertyAttributeNone,
                        nullptr);
    JSObjectSetProperty(ctx, object, JSStringCreateWithUTF8CString("value"),
                        JSValueMakeNumber(ctx, value.value),
                        kJSPropertyAttributeNone, nullptr);
    return object;
}

// Alignment

elements::EdgeInsets alignment_from_js(JSContextRef ctx, JSObjectRef object) {
    elements::EdgeInsets alignment;
    map_prop_from_js<Value, value_from_js>(ctx, object, "left",
                                           &alignment.left);
    map_prop_from_js<Value, value_from_js>(ctx, object, "top", &alignment.top);
    map_prop_from_js<Value, value_from_js>(ctx, object, "right",
                                           &alignment.right);
    map_prop_from_js<Value, value_from_js>(ctx, object, "bottom",
                                           &alignment.bottom);
    return alignment;
}

JSObjectRef alignment_to_js(JSContextRef ctx,
                            const elements::EdgeInsets& alignment) {
    auto object = JSObjectMake(ctx, nullptr, nullptr);
    map_prop_to_js<Value, value_to_js>(ctx, object, "left", alignment.left);
    map_prop_to_js<Value, value_to_js>(ctx, object, "top", alignment.top);
    map_prop_to_js<Value, value_to_js>(ctx, object, "right", alignment.right);
    map_prop_to_js<Value, value_to_js>(ctx, object, "bottom", alignment.bottom);
    return object;
}

// SizeConstraints

elements::SizeConstraints size_constraints_from_js(JSContextRef ctx,
                                                   JSObjectRef object) {
    elements::SizeConstraints size;
    map_prop_from_js<Value, value_from_js>(ctx, object, "maxWidth",
                                           &size.max_width);
    map_prop_from_js<Value, value_from_js>(ctx, object, "maxHeight",
                                           &size.max_height);
    map_prop_from_js<Value, value_from_js>(ctx, object, "minWidth",
                                           &size.min_width);
    map_prop_from_js<Value, value_from_js>(ctx, object, "minHeight",
                                           &size.min_height);
    return size;
}

JSObjectRef size_constraints_to_js(JSContextRef ctx,
                                   const elements::SizeConstraints& size) {
    auto object = JSObjectMake(ctx, nullptr, nullptr);
    map_prop_to_js<Value, value_to_js>(ctx, object, "maxWidth", size.max_width);
    map_prop_to_js<Value, value_to_js>(ctx, object, "maxHeight",
                                       size.max_height);
    map_prop_to_js<Value, value_to_js>(ctx, object, "minWidth", size.min_width);
    map_prop_to_js<Value, value_to_js>(ctx, object, "minHeight",
                                       size.min_height);
    return object;
}

}  // namespace aardvark::js
