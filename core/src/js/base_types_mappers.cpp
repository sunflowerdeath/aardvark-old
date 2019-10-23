#include "base_types_mappers.hpp"

#define GET(name) [](auto* obj) { return &(obj->name); }

namespace aardvark::js {

Mapper<int>* int_mapper = new SimpleMapper<int, int_to_js, int_from_js>();
Mapper<float>* float_mapper =
    new SimpleMapper<float, float_to_js, float_from_js>();
Mapper<std::string>* str_mapper =
    new SimpleMapper<std::string, str_to_js, str_from_js>();
Mapper<UnicodeString>* icu_str_mapper =
    new SimpleMapper<UnicodeString, icu_str_to_js, icu_str_from_js>();

Mapper<Size>* size_mapper = new ObjectMapper<Size>(
    {new PropMapper<Size, float>(float_mapper, "width", GET(width)),
     new PropMapper<Size, float>(float_mapper, "height", GET(height))});

Mapper<Position>* position_mapper = new ObjectMapper<Position>(
    {new PropMapper<Position, float>(float_mapper, "left", GET(left)),
     new PropMapper<Position, float>(float_mapper, "top", GET(top))});

using ValueType = Value::ValueType;
auto value_type_mapper = new EnumMapper<ValueType>(int_mapper);
Mapper<Value>* value_mapper = new ObjectMapper<Value>(
    {new PropMapper<Value, ValueType>(value_type_mapper, "type", GET(type)),
     new PropMapper<Value, float>(float_mapper, "value", GET(value))});

Mapper<BoxConstraints>* box_constraints_mapper =
    new ObjectMapper<BoxConstraints>(
        {new PropMapper<BoxConstraints, float>(float_mapper, "minWidth",
                                               GET(min_width)),
         new PropMapper<BoxConstraints, float>(float_mapper, "maxWidth",
                                               GET(max_width)),
         new PropMapper<BoxConstraints, float>(float_mapper, "minHeight",
                                               GET(min_height)),
         new PropMapper<BoxConstraints, float>(float_mapper, "maxHeight",
                                               GET(max_height))});

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
Mapper<Alignment>* alignment_mapper = new ObjectMapper<Alignment>(
    {new PropMapper<Alignment, Value>(value_mapper, "left", GET(left)),
     new PropMapper<Alignment, Value>(value_mapper, "top", GET(top)),
     new PropMapper<Alignment, Value>(value_mapper, "right", GET(right)),
     new PropMapper<Alignment, Value>(value_mapper, "bottom", GET(bottom))});

Mapper<Padding>* padding_mapper = new ObjectMapper<Padding>(
    {new PropMapper<Padding, float>(float_mapper, "left", GET(left)),
     new PropMapper<Padding, float>(float_mapper, "top", GET(top)),
     new PropMapper<Padding, float>(float_mapper, "right", GET(right)),
     new PropMapper<Padding, float>(float_mapper, "bottom", GET(bottom))});

using SizeConstraints = elements::SizeConstraints;
Mapper<SizeConstraints>* size_constraints_mapper =
    new ObjectMapper<SizeConstraints>(
        {new PropMapper<SizeConstraints, Value>(value_mapper, "width",
                                                GET(width)),
         new PropMapper<SizeConstraints, Value>(value_mapper, "height",
                                                GET(height)),
         new PropMapper<SizeConstraints, Value>(value_mapper, "minWidth",
                                                GET(min_width)),
         new PropMapper<SizeConstraints, Value>(value_mapper, "minHeight",
                                                GET(min_height)),
         new PropMapper<SizeConstraints, Value>(value_mapper, "maxWidth",
                                                GET(max_width)),
         new PropMapper<SizeConstraints, Value>(value_mapper, "maxHeight",
                                                GET(max_height))});

auto pointer_tool_mapper = new EnumMapper<PointerTool>(int_mapper);
auto pointer_action_mapper = new EnumMapper<PointerAction>(int_mapper);
Mapper<PointerEvent>* pointer_event_mapper = new ObjectMapper<PointerEvent>(
    {new PropMapper<PointerEvent, int>(int_mapper, "timestamp", GET(timestamp)),
     new PropMapper<PointerEvent, int>(int_mapper, "pointerId",
                                       GET(pointer_id)),
     new PropMapper<PointerEvent, PointerTool>(pointer_tool_mapper, "tool",
                                               GET(tool)),
     new PropMapper<PointerEvent, PointerAction>(pointer_action_mapper,
                                                 "action", GET(action)),
     new PropMapper<PointerEvent, float>(float_mapper, "left", GET(left)),
     new PropMapper<PointerEvent, float>(float_mapper, "top", GET(top))});

auto key_action_mapper = new EnumMapper<KeyAction>(int_mapper);
Mapper<KeyEvent>* key_event_mapper = new ObjectMapper<KeyEvent>(
    {new PropMapper<KeyEvent, int>(int_mapper, "key", GET(key)),
     new PropMapper<KeyEvent, int>(int_mapper, "scancode", GET(scancode)),
     new PropMapper<KeyEvent, KeyAction>(key_action_mapper, "action",
                                         GET(action))});

Mapper<ScrollEvent>* scroll_event_mapper = new ObjectMapper<ScrollEvent>(
    {new PropMapper<ScrollEvent, float>(float_mapper, "top", GET(top)),
     new PropMapper<ScrollEvent, float>(float_mapper, "left", GET(left))});

}  // namespace aardvark::js
