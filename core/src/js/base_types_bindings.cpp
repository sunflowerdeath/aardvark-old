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
        value = static_cast<float>(JSValueToNumber(ctx, value_prop, nullptr));
    }
    return Value(type, value);
}

void set_alignment_side(JSContextRef ctx, JSObjectRef object,
                        const char* side_name, Value* side) {
    auto prop_name = JSStringCreateWithUTF8CString(side_name);
    if (JSObjectHasProperty(ctx, object, prop_name)) {
        auto side_prop = JSObjectGetProperty(ctx, object, prop_name, nullptr);
        *side = value_from_js(ctx, JSValueToObject(ctx, side_prop, nullptr));
    }
}

elements::EdgeInsets alignment_from_js(JSContextRef ctx, JSObjectRef object) {
    elements::EdgeInsets insets;
    set_alignment_side(ctx, object, "left", &insets.left);
    set_alignment_side(ctx, object, "top", &insets.top);
    set_alignment_side(ctx, object, "right", &insets.right);
    set_alignment_side(ctx, object, "bottom", &insets.bottom);
    return insets;
}

}  // namespace aardvark::js
