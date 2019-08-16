#include "desktop_app_bindings.hpp"
#include "../platforms/desktop/desktop_app.hpp"
#include "bindings_host.hpp"

namespace aardvark::js {

JSValueRef desktop_app_run(JSContextRef ctx, JSObjectRef function,
                           JSObjectRef object, size_t argument_count,
                           const JSValueRef arguments[],
                           JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto app = host->desktop_app_index->get_native_object(object);
    app->run(std::bind(&EventLoop::execute_callbacks, &host->event_loop));
    return JSValueMakeUndefined(ctx);
}

JSValueRef desktop_app_create_window(JSContextRef ctx, JSObjectRef function,
                                     JSObjectRef object, size_t argument_count,
                                     const JSValueRef arguments[],
                                     JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto app = host->desktop_app_index->get_native_object(object);
    auto width = static_cast<float>(JSValueToNumber(ctx, arguments[0], 0));
    auto height = static_cast<float>(JSValueToNumber(ctx, arguments[1], 0));
    auto window = app->create_window(Size{width, height});
    return host->desktop_window_index->create_js_object(window);
}

JSValueRef desktop_app_stop(JSContextRef ctx, JSObjectRef function,
                            JSObjectRef object, size_t argument_count,
                            const JSValueRef arguments[],
                            JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto app = host->desktop_app_index->get_native_object(object);
    app->stop();
    return JSValueMakeUndefined(ctx);
}

JSValueRef desktop_app_get_document(JSContextRef ctx, JSObjectRef function,
                                    JSObjectRef object, size_t argument_count,
                                    const JSValueRef arguments[],
                                    JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto app = host->desktop_app_index->get_native_object(object);
    auto window = host->desktop_window_index->get_native_object(
        JSValueToObject(ctx, arguments[0], 0));
    auto doc = app->get_document(window);
    return host->document_index->get_or_create_js_object(doc);
}

JSValueRef desktop_app_get_windows(JSContextRef ctx, JSObjectRef object,
                                   JSStringRef property_name,
                                   JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto app = host->desktop_app_index->get_native_object(object);
    return host->desktop_app_window_list_index->get_or_create_js_object(app);
}

void desktop_app_finalize(JSObjectRef object) {
    ObjectsIndex<DesktopApp>::remove(object);
}

JSClassRef desktop_app_create_class() {
    auto definition = kJSClassDefinitionEmpty;
    JSStaticFunction static_functions[] = {
        {"createWindow", desktop_app_create_window, PROP_ATTR_STATIC},
        {"getDocument", desktop_app_get_document, PROP_ATTR_STATIC},
        {"run", desktop_app_run, PROP_ATTR_STATIC},
        {"stop", desktop_app_stop, PROP_ATTR_STATIC},
        {0, 0, 0}};
    JSStaticValue static_values[] = {
        {"windows", desktop_app_get_windows, nullptr, PROP_ATTR_STATIC},
        {0, 0, 0, 0}};
    definition.className = "DesktopApp";
    definition.staticFunctions = static_functions;
    definition.staticValues = static_values;
    definition.finalize = desktop_app_finalize;
    return JSClassCreate(&definition);
};

JSObjectRef desktop_app_call_as_constructor(JSContextRef ctx,
                                            JSObjectRef constructor,
                                            size_t argumentCount,
                                            const JSValueRef arguments[],
                                            JSValueRef* exception) {
    auto app = std::make_shared<aardvark::DesktopApp>();
    return BindingsHost::get(ctx)->desktop_app_index->create_js_object(app);
}

// DesktopAppWindowsList

bool is_number(const std::string& str) {
    return std::all_of(str.begin(), str.end(), ::isdigit);
}

bool js_string_is_index(JSStringRef js_string, int max_index) {
    auto string = jsstring_to_std(js_string);
    return is_number(string) && std::stoi(string) < max_index;
}

bool desktop_app_window_list_has_property(JSContextRef ctx, JSObjectRef object,
                                          JSStringRef property_name) {
    auto host = BindingsHost::get(ctx);
    auto app = host->desktop_app_window_list_index->get_native_object(object);
    return js_string_is_index(property_name, app->windows.size());
}

JSValueRef desktop_app_window_list_get_property(JSContextRef ctx,
                                                JSObjectRef object,
                                                JSStringRef property_name,
                                                JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto app = host->desktop_app_window_list_index->get_native_object(object);
    auto index = std::stoi(jsstring_to_std(property_name));
    return host->desktop_window_index->get_or_create_js_object(
        app->windows[index]);
}

JSValueRef desktop_app_window_list_get_size(JSContextRef ctx,
                                            JSObjectRef object,
                                            JSStringRef property_name,
                                            JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto app = host->desktop_app_window_list_index->get_native_object(object);
    return JSValueMakeNumber(ctx, static_cast<double>(app->windows.size()));
}

void desktop_app_window_list_finalize(JSObjectRef object) {
    ObjectsIndex<DesktopApp>::remove(object);
}

JSClassRef desktop_app_window_list_create_class() {
    auto definition = kJSClassDefinitionEmpty;
    JSStaticValue static_values[] = {{"size", desktop_app_window_list_get_size,
                                      nullptr, property_attributes_immutable},
                                     {0, 0, 0, 0}};
    definition.className = "DesktopAppWindowsList";
    definition.staticValues = static_values;
    definition.finalize = desktop_app_window_list_finalize;
    definition.hasProperty = desktop_app_window_list_has_property;
    definition.getProperty = desktop_app_window_list_get_property;
    return JSClassCreate(&definition);
};

}  // namespace aardvark::js
