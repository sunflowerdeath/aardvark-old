#include "desktop_app_bindings.hpp"
#include "bindings_host.hpp"
#include "../platforms/desktop/desktop_app.hpp"

namespace aardvark::js {

JSValueRef desktop_app_run(JSContextRef ctx, JSObjectRef function,
                           JSObjectRef object, size_t argument_count,
                           const JSValueRef arguments[],
                           JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto app = host->desktop_app_index->get_native_object(object);
    app->run();
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
    // auto app = get_host(ctx).desktop_app_index.get_instance(object);
    return JSValueMakeNumber(ctx, static_cast<double>(123));
}

void desktop_app_finalize(JSObjectRef object) {
    ObjectsIndex<DesktopApp>::remove(object);
}

JSClassRef desktop_app_create_class() {
    auto definition = kJSClassDefinitionEmpty;
    JSStaticFunction static_functions[] = {
        JSStaticFunction{"createWindow", desktop_app_create_window,
                         property_attributes_immutable},
        JSStaticFunction{"getDocument", desktop_app_get_document,
                         property_attributes_immutable},
        JSStaticFunction{"run", desktop_app_run, property_attributes_immutable},
        JSStaticFunction{"stop", desktop_app_stop,
                         property_attributes_immutable},
        JSStaticFunction{0, 0, 0}};
    JSStaticValue static_values[] = {{"windows", desktop_app_get_windows,
                                      nullptr, property_attributes_immutable},
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

}
