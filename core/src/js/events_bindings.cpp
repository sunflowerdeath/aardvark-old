#include "events_bindings.hpp"

namespace aardvark::js {

template <typename T>
void set_num_prop(JSContextRef ctx, JSObjectRef object, char* name, T value) {
    auto str = JSStringCreateWithUTF8CString(name);
    JSObjectSetProperty(ctx, object,
                        str,                            // propertyName
                        JSValueMakeNumber(ctx, value),  // value,
                        kJSPropertyAttributeNone,       // attributes
                        nullptr                         // exception
    );
    JSStringRelease(str);
}

JSValueRef pointer_event_to_js(JSContextRef ctx, const PointerEvent& event) {
    auto object = JSObjectMake(ctx, /* jsClass */ nullptr, /* data */ nullptr);

    set_num_prop(ctx, object, "pointerId", event.pointer_id);

    auto tool = static_cast<std::underlying_type_t<PointerTool>>(event.tool);
    set_num_prop(ctx, object, "tool", tool);

    auto action =
        static_cast<std::underlying_type_t<PointerAction>>(event.action);
    set_num_prop(ctx, object, "action", action);

    set_num_prop(ctx, object, "left", event.left);
    set_num_prop(ctx, object, "top", event.top);

    return object;
}

}
