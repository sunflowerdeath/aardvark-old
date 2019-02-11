#include "base_types_bindings.hpp"

namespace aardvark::js {

Value value_from_js(JSContextRef ctx, JSObjectRef object) {
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

JSObjectRef value_to_js(JSContextRef ctx, const Value& value) {
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

void alignment_side_from_js(JSContextRef ctx, JSObjectRef object,
                            const char* side_name, Value* side) {
    auto prop_name = JSStringCreateWithUTF8CString(side_name);
    if (JSObjectHasProperty(ctx, object, prop_name)) {
        auto side_prop = JSObjectGetProperty(ctx, object, prop_name, nullptr);
        *side = value_from_js(ctx, JSValueToObject(ctx, side_prop, nullptr));
    }
}

elements::EdgeInsets alignment_from_js(JSContextRef ctx, JSObjectRef object) {
    elements::EdgeInsets alignment;
    alignment_side_from_js(ctx, object, "left", &alignment.left);
    alignment_side_from_js(ctx, object, "top", &alignment.top);
    alignment_side_from_js(ctx, object, "right", &alignment.right);
    alignment_side_from_js(ctx, object, "bottom", &alignment.bottom);
    return alignment;
}

void alignment_side_to_js(JSContextRef ctx, JSObjectRef object,
                          const char* side_name, const Value& side) {
    JSObjectSetProperty(ctx, object, JSStringCreateWithUTF8CString(side_name),
                        value_to_js(ctx, side), kJSPropertyAttributeNone,
                        nullptr);
}

JSObjectRef alignment_to_js(JSContextRef ctx,
                            const elements::EdgeInsets& alignment) {
    auto object = JSObjectMake(ctx, nullptr, nullptr);
    alignment_side_to_js(ctx, object, "left", alignment.left);
    alignment_side_to_js(ctx, object, "top", alignment.top);
    alignment_side_to_js(ctx, object, "right", alignment.right);
    alignment_side_to_js(ctx, object, "bottom", alignment.bottom);
    return object;
}

}  // namespace aardvark::js
