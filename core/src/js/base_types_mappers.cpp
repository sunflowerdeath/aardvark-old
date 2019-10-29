#include "base_types_mappers.hpp"

namespace aardvark::js {

Mapper<int>* int_mapper = new SimpleMapper<int, int_to_js, int_from_js>();
Mapper<float>* float_mapper =
    new SimpleMapper<float, float_to_js, float_from_js>();
Mapper<std::string>* str_mapper =
    new SimpleMapper<std::string, str_to_js, str_from_js>();
Mapper<UnicodeString>* icu_str_mapper =
    new SimpleMapper<UnicodeString, icu_str_to_js, icu_str_from_js>();

Mapper<Size>* size_mapper = new ObjectMapper<Size, float, float>(
    {{"width", &Size::width, float_mapper},
     {"height", &Size::height, float_mapper}});

Mapper<Position>* position_mapper = new ObjectMapper<Position, float, float>(
    {{"left", &Position::left, float_mapper},
     {"top", &Position::top, float_mapper}});

using ValueType = Value::ValueType;
auto value_type_mapper = new EnumMapper<ValueType>(int_mapper);
Mapper<Value>* value_mapper = new ObjectMapper<Value, ValueType, float>(
    {{"type", &Value::type, value_type_mapper},
     {"value", &Value::value, float_mapper}});

Mapper<BoxConstraints>* box_constraints_mapper =
    new ObjectMapper<BoxConstraints, float, float, float, float>(
        {{"minWidth", &BoxConstraints::min_width, float_mapper},
         {"maxWidth", &BoxConstraints::max_width, float_mapper},
         {"minHeight", &BoxConstraints::min_height, float_mapper},
         {"maxHeight", &BoxConstraints::max_height, float_mapper}});

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

Mapper<SkColor>* color_mapper =
    new SimpleMapper<SkColor, color_to_js, color_from_js>();

using Alignment = elements::EdgeInsets;
Mapper<Alignment>* alignment_mapper =
    new ObjectMapper<Alignment, Value, Value, Value, Value>(
        {{"left", &Alignment::left, value_mapper},
         {"top", &Alignment::top, value_mapper},
         {"right", &Alignment::right, value_mapper},
         {"bottom", &Alignment::bottom, value_mapper}});

Mapper<Padding>* padding_mapper =
    new ObjectMapper<Padding, float, float, float, float>(
        {{"left", &Padding::left, float_mapper},
         {"top", &Padding::top, float_mapper},
         {"right", &Padding::right, float_mapper},
         {"bottom", &Padding::bottom, float_mapper}});

using SizeConstraints = elements::SizeConstraints;
Mapper<SizeConstraints>* size_constraints_mapper =
    new ObjectMapper<SizeConstraints, Value, Value, Value, Value, Value, Value>(
        {"width", &SizeConstraints::width, value_mapper},
        {"height", &SizeConstraints::height, value_mapper},
        {"minWidth", &SizeConstraints::min_width, value_mapper},
        {"minHeight", &SizeConstraints::min_height, value_mapper},
        {"maxWidth", &SizeConstraints::max_width, value_mapper},
        {"maxHeight", &SizeConstraints::max_height, value_mapper});

auto pointer_tool_mapper = new EnumMapper<PointerTool>(int_mapper);
auto pointer_action_mapper = new EnumMapper<PointerAction>(int_mapper);
Mapper<PointerEvent>* pointer_event_mapper =
    new ObjectMapper<PointerEvent, int, int, PointerTool, PointerAction, float,
                     float>(
        {{"timestamp", &PointerEvent::timestamp, int_mapper},
         {"pointerId", &PointerEvent::pointer_id, int_mapper},
         {"tool", &PointerEvent::tool, pointer_tool_mapper},
         {"action", &PointerEvent::action, pointer_action_mapper},
         {"left", &PointerEvent::left, float_mapper},
         {"top", &PointerEvent::top, float_mapper}});

auto key_action_mapper = new EnumMapper<KeyAction>(int_mapper);
Mapper<KeyEvent>* key_event_mapper =
    new ObjectMapper<KeyEvent, int, int, KeyAction>(
        {{"key", &KeyEvent::key, int_mapper},
         {"scancode", &KeyEvent::scancode, int_mapper},
         {"action", &KeyEvent::action, key_action_mapper}});

Mapper<ScrollEvent>* scroll_event_mapper =
    new ObjectMapper<ScrollEvent, float, float>(
        {{"left", &ScrollEvent::left, float_mapper},
         {"top", &ScrollEvent::top, float_mapper}});

}  // namespace aardvark::js
