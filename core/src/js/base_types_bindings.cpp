#include "base_types_bindings.hpp"

#include "helpers.hpp"

namespace aardvark::js {

// Color

SkColor color_from_js(JSContextRef ctx, JSValueRef js_value) {
    auto object = JSValueToObject(ctx, js_value, nullptr);
    int alpha = 0;
    int red = 0;
    int green = 0;
    int blue = 0;
    map_prop_from_js<int, int_from_js>(ctx, object, "alpha", &alpha);
    map_prop_from_js<int, int_from_js>(ctx, object, "red", &red);
    map_prop_from_js<int, int_from_js>(ctx, object, "green", &green);
    map_prop_from_js<int, int_from_js>(ctx, object, "blue", &blue);
    return SkColorSetARGB(alpha, red, green, blue);
}

JSValueRef color_to_js(JSContextRef ctx, SkColor color) {
    auto object = JSObjectMake(ctx, nullptr, nullptr);
    map_prop_to_js<int, int_to_js>(ctx, object, "alpha", SkColorGetA(color));
    map_prop_to_js<int, int_to_js>(ctx, object, "red", SkColorGetR(color));
    map_prop_to_js<int, int_to_js>(ctx, object, "green", SkColorGetG(color));
    map_prop_to_js<int, int_to_js>(ctx, object, "blue", SkColorGetB(color));
    return object;
}

// Value

Value value_from_js(JSContextRef ctx, JSValueRef js_value) {
    auto object = JSValueToObject(ctx, js_value, nullptr);

    auto type_prop = JSObjectGetProperty(
        ctx, object, JsStringCache::get("type"), nullptr);
    auto type_str = JSValueToStringCopy(ctx, type_prop, nullptr);
    Value::ValueType type;
    if (JSStringIsEqualToUTF8CString(type_str, "abs")) {
        type = Value::ValueType::abs;
    } else if (JSStringIsEqualToUTF8CString(type_str, "rel")) {
        type = Value::ValueType::rel;
    } else {
        type = Value::ValueType::none;
    }
    JSStringRelease(type_str);

    float value = 0;
    if (type != Value::ValueType::none) {
        auto value_prop = JSObjectGetProperty(
            ctx, object, JsStringCache::get("value"), nullptr);
        value = JSValueToNumber(ctx, value_prop, nullptr);
    }

    return Value(type, value);
}

JSValueRef value_to_js(JSContextRef ctx, const Value& value) {
    auto object = JSObjectMake(ctx, nullptr, nullptr);

    JSStringRef type_str;
    if (value.type == Value::ValueType::abs) {
        type_str = JSStringCreateWithUTF8CString("abs");
    } else if (value.type == Value::ValueType::rel) {
        type_str = JSStringCreateWithUTF8CString("rel");
    } else {
        type_str = JSStringCreateWithUTF8CString("none");
    }
    JSObjectSetProperty(ctx, object, JsStringCache::get("type"),
                        JSValueMakeString(ctx, type_str),
                        kJSPropertyAttributeNone, nullptr);
    JSStringRelease(type_str);

    JSObjectSetProperty(ctx, object, JsStringCache::get("value"),
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
