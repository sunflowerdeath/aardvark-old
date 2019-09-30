#include "events_bindings.hpp"

#include "helpers.hpp"

namespace aardvark::js {

JSValueRef pointer_event_to_js(JSContextRef ctx, const PointerEvent& event) {
    auto object = JSObjectMake(ctx, /* jsClass */ nullptr, /* data */ nullptr);

    map_prop_to_js<int, int_to_js>(ctx, object, "timestamp", event.timestamp);
    map_prop_to_js<int, int_to_js>(ctx, object, "pointerId",
                                     event.pointer_id);

    auto tool = static_cast<std::underlying_type_t<PointerTool>>(event.tool);
    map_prop_to_js<int, int_to_js>(ctx, object, "tool", tool);

    auto action =
        static_cast<std::underlying_type_t<PointerAction>>(event.action);
    map_prop_to_js<int, int_to_js>(ctx, object, "action", action);

    map_prop_to_js<int, int_to_js>(ctx, object, "left", event.left);
    map_prop_to_js<int, int_to_js>(ctx, object, "top", event.top);

    return object;
}

}
