#include "base_types_mappers.hpp"

#define MAPPER(NAME, TYPE, DEFINITION)   \
    Mapper<TYPE>* NAME() {               \
        static auto mapper = DEFINITION; \
        return &mapper;                  \
    }

namespace aardvark::js {

MAPPER(int_mapper, int, (SimpleMapper<int, int_to_js, int_from_js>()));

MAPPER(float_mapper, float,
       (SimpleMapper<float, float_to_js, float_from_js>()));

MAPPER(str_mapper, std::string,
       (SimpleMapper<std::string, str_to_js, str_from_js>()));

MAPPER(icu_str_mapper, UnicodeString,
       (SimpleMapper<UnicodeString, icu_str_to_js, icu_str_from_js>()));

MAPPER(size_mapper, Size,
       ObjectMapper<Size>(
           {ADV_PROP_MAPPER(Size, float, float_mapper(), "width", width),
            ADV_PROP_MAPPER(Size, float, float_mapper(), "height", height)}));

MAPPER(position_mapper, Position,
       ObjectMapper<Position>(
           {ADV_PROP_MAPPER(Position, float, float_mapper(), "left", left),
            ADV_PROP_MAPPER(Position, float, float_mapper(), "top", top)}));

MAPPER(value_type_mapper, Value::ValueType,
       EnumMapper<Value::ValueType>(int_mapper()));

MAPPER(value_mapper, Value,
       ObjectMapper<Value>({ADV_PROP_MAPPER(Value, Value::ValueType,
                                            value_type_mapper(), "type", type),
                            ADV_PROP_MAPPER(Value, float, float_mapper(),
                                            "value", value)}));

MAPPER(box_constraints_mapper, BoxConstraints,
       ObjectMapper<BoxConstraints>(
           {ADV_PROP_MAPPER(BoxConstraints, float, float_mapper(), "minWidth",
                            min_width),
            ADV_PROP_MAPPER(BoxConstraints, float, float_mapper(), "maxWidth",
                            max_width),
            ADV_PROP_MAPPER(BoxConstraints, float, float_mapper(), "minHeight",
                            min_height),
            ADV_PROP_MAPPER(BoxConstraints, float, float_mapper(), "maxHeight",
                            max_height)}));

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

JSValueRef color_to_js(JSContextRef ctx, const SkColor& color) {
    auto object = JSObjectMake(ctx, nullptr, nullptr);
    map_prop_to_js<int, int_to_js>(ctx, object, "alpha", SkColorGetA(color));
    map_prop_to_js<int, int_to_js>(ctx, object, "red", SkColorGetR(color));
    map_prop_to_js<int, int_to_js>(ctx, object, "green", SkColorGetG(color));
    map_prop_to_js<int, int_to_js>(ctx, object, "blue", SkColorGetB(color));
    return object;
}

MAPPER(color_mapper, SkColor,
       (SimpleMapper<SkColor, color_to_js, color_from_js>()));

MAPPER(alignment_mapper, elements::EdgeInsets,
       ObjectMapper<elements::EdgeInsets>(
           {ADV_PROP_MAPPER(elements::EdgeInsets, Value, value_mapper(), "left",
                            left),
            ADV_PROP_MAPPER(elements::EdgeInsets, Value, value_mapper(), "top",
                            top),
            ADV_PROP_MAPPER(elements::EdgeInsets, Value, value_mapper(),
                            "right", right),
            ADV_PROP_MAPPER(elements::EdgeInsets, Value, value_mapper(),
                            "bottom", bottom)}));

MAPPER(padding_mapper, Padding,
       ObjectMapper<Padding>(
           {ADV_PROP_MAPPER(Padding, float, float_mapper(), "left", left),
            ADV_PROP_MAPPER(Padding, float, float_mapper(), "top", top),
            ADV_PROP_MAPPER(Padding, float, float_mapper(), "right", right),
            ADV_PROP_MAPPER(Padding, float, float_mapper(), "bottom",
                            bottom)}));

MAPPER(size_constraints_mapper, elements::SizeConstraints,
       ObjectMapper<elements::SizeConstraints>({
           ADV_PROP_MAPPER(elements::SizeConstraints, Value, value_mapper(),
                           "width", width),
           ADV_PROP_MAPPER(elements::SizeConstraints, Value, value_mapper(),
                           "height", height),
           ADV_PROP_MAPPER(elements::SizeConstraints, Value, value_mapper(),
                           "minWidth", min_width),
           ADV_PROP_MAPPER(elements::SizeConstraints, Value, value_mapper(),
                           "minHeight", min_height),
           ADV_PROP_MAPPER(elements::SizeConstraints, Value, value_mapper(),
                           "maxWidth", max_width),
           ADV_PROP_MAPPER(elements::SizeConstraints, Value, value_mapper(),
                           "maxHeight", max_height),
       }));

MAPPER(pointer_tool_mapper, PointerTool,
       EnumMapper<PointerTool>(int_mapper()));

MAPPER(pointer_action_mapper, PointerAction,
       EnumMapper<PointerAction>(int_mapper()));

MAPPER(pointer_event_mapper, PointerEvent,
       ObjectMapper<PointerEvent>(
           {ADV_PROP_MAPPER(PointerEvent, int, int_mapper(), "timestamp",
                            timestamp),
            ADV_PROP_MAPPER(PointerEvent, int, int_mapper(), "pointerId",
                            pointer_id),
            ADV_PROP_MAPPER(PointerEvent, PointerTool, pointer_tool_mapper(),
                            "tool", tool),
            ADV_PROP_MAPPER(PointerEvent, PointerAction,
                            pointer_action_mapper(), "action", action),
            ADV_PROP_MAPPER(PointerEvent, float, float_mapper(), "left", left),
            ADV_PROP_MAPPER(PointerEvent, float, float_mapper(), "top", top)}));

MAPPER(key_action_mapper, KeyAction, EnumMapper<KeyAction>(int_mapper()));

MAPPER(key_event_mapper, KeyEvent,
       ObjectMapper<KeyEvent>(
           {ADV_PROP_MAPPER(KeyEvent, int, int_mapper(), "key", key),
            ADV_PROP_MAPPER(KeyEvent, int, int_mapper(), "scancode", scancode),
            ADV_PROP_MAPPER(KeyEvent, KeyAction, key_action_mapper(), "action",
                            action)}));

MAPPER(scroll_event_mapper, ScrollEvent,
       ObjectMapper<ScrollEvent>(
           {ADV_PROP_MAPPER(ScrollEvent, float, float_mapper(), "top", top),
            ADV_PROP_MAPPER(ScrollEvent, float, float_mapper(), "left",
                            left)}));

}  // namespace aardvark::js
