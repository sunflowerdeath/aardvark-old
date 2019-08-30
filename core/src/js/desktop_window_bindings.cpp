#include "desktop_window_bindings.hpp"
#include "bindings_host.hpp"
#include "../platforms/desktop/desktop_window.hpp"

namespace aardvark::js {

JSValueRef desktop_window_get_width(JSContextRef ctx, JSObjectRef object,
                                   JSStringRef propertyName,
                                   JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto window = host->desktop_window_index->get_native_object(object);
    return JSValueMakeNumber(ctx, window->size.width);
}

JSValueRef desktop_window_get_height(JSContextRef ctx, JSObjectRef object,
                                   JSStringRef propertyName,
                                   JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto window = host->desktop_window_index->get_native_object(object);
    return JSValueMakeNumber(ctx, window->size.height);
}

void desktop_window_finalize(JSObjectRef object) {
    ObjectsIndex<DesktopWindow>::remove(object);
}

JSClassRef desktop_window_create_class() {
    auto definition = kJSClassDefinitionEmpty;
    JSStaticValue static_values[] = {
        {"width", desktop_window_get_width, nullptr, PROP_ATTR_STATIC},
        {"height", desktop_window_get_height, nullptr, PROP_ATTR_STATIC},
        {0, 0, 0, 0}};
    definition.className = "DesktopWindow";
    definition.finalize = desktop_window_finalize;
    definition.staticValues = static_values;
    return JSClassCreate(&definition);
}

}  // namespace aardvark::js
