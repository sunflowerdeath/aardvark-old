#include "bindings_host.hpp"

namespace aardvark::js {

BindingsHost* get_bindings_host(JSContextRef ctx) {
    auto global_object = JSContextGetGlobalObject(ctx);
    return static_cast<BindingsHost*>(JSObjectGetPrivate(global_object));
}

// log

std::string jsstring_to_std(JSStringRef jsstring) {
    auto size = JSStringGetMaximumUTF8CStringSize(jsstring);
    auto buffer = new char[size];
    JSStringGetUTF8CString(jsstring, buffer, size);
    auto stdstring = std::string(buffer);
    delete[] buffer;
    return stdstring;
};

JSValueRef log(
    JSContextRef ctx, JSObjectRef function, JSObjectRef this_object,
    size_t argument_count, const JSValueRef arguments[], JSValueRef* exception
) {
    for (auto i = 0; i < argument_count; i++) {
        if (i != 0) std::cout <<  " ";
        auto str = JSValueToStringCopy(ctx, arguments[i], nullptr);
        std::cout << jsstring_to_std(str) << std::endl;
        JSStringRelease(str);
    }
    std::cout << std::endl;
    return JSValueMakeUndefined(ctx);
};

// DesktopApp
void desktop_app_finalize(JSObjectRef object) {
    // TODO finalize
    // get_bindings_host(ctx)->desktop_app_index.remove(object);
}

JSValueRef desktop_app_run(JSContextRef ctx, JSObjectRef function,
                           JSObjectRef object, size_t argument_count,
                           const JSValueRef arguments[],
                           JSValueRef* exception) {
    auto app = get_bindings_host(ctx)->desktop_app_index->get_instance(object);
    app->run();
    return JSValueMakeUndefined(ctx);
};

JSValueRef desktop_app_create_window(JSContextRef ctx, JSObjectRef function,
                                     JSObjectRef object, size_t argument_count,
                                     const JSValueRef arguments[],
                                     JSValueRef* exception) {
    auto host = get_bindings_host(ctx);
    auto app = host->desktop_app_index->get_instance(object);
    auto width = static_cast<float>(JSValueToNumber(ctx, arguments[0], 0));
    auto height = static_cast<float>(JSValueToNumber(ctx, arguments[1], 0));
    auto window = app->create_window(Size{width, height});
    return host->desktop_window_index->get_or_create_object(window);
};

JSValueRef desktop_app_stop(JSContextRef ctx, JSObjectRef function,
                           JSObjectRef object, size_t argument_count,
                           const JSValueRef arguments[],
                           JSValueRef* exception) {
    auto app = get_bindings_host(ctx)->desktop_app_index->get_instance(object);
    app->stop();
    return JSValueMakeUndefined(ctx);
};

JSValueRef desktop_app_get_windows(JSContextRef ctx, JSObjectRef object,
                                   JSStringRef property_name,
                                   JSValueRef* exception) {
    // auto app = get_host(ctx).desktop_app_index.get_instance(object);
    return JSValueMakeNumber(ctx, static_cast<double>(123));
};

JSClassRef desktop_app_create_class() {
    auto definition = kJSClassDefinitionEmpty;
    JSStaticFunction static_functions[] = {
        JSStaticFunction{"createWindow", desktop_app_create_window,
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
    return get_bindings_host(ctx)->desktop_app_index->get_or_create_object(app);
}

// DesktopWindow

void desktop_window_finalize(JSObjectRef object) {
    // ?
    // get_bindings_host(ctx)->desktop_window_index.remove(object);
}

JSValueRef desktop_window_get_width(JSContextRef ctx, JSObjectRef object,
                                   JSStringRef propertyName,
                                   JSValueRef* exception) {
    auto host = get_bindings_host(ctx);
    auto window = host->desktop_window_index->get_instance(object);
    return JSValueMakeNumber(ctx, window->size.width);
}

JSValueRef desktop_window_get_height(JSContextRef ctx, JSObjectRef object,
                                   JSStringRef propertyName,
                                   JSValueRef* exception) {
    auto host = get_bindings_host(ctx);
    auto window = host->desktop_window_index->get_instance(object);
    return JSValueMakeNumber(ctx, window->size.height);
}

JSClassRef desktop_window_create_class() {
    auto definition = kJSClassDefinitionEmpty;
    JSStaticValue static_values[] = {{"width", desktop_window_get_width,
                                      nullptr, property_attributes_immutable},
                                     {"height", desktop_window_get_height,
                                      nullptr, property_attributes_immutable},
                                     {0, 0, 0, 0}};
    definition.className = "DesktopWindow";
    definition.finalize = desktop_window_finalize;
    definition.staticValues = static_values;
    return JSClassCreate(&definition);
}

// BindingsHost

BindingsHost::BindingsHost() {
    // Create empty dummy class because default object class does not allocate 
    // storage for private data
    auto global_class = JSClassCreate(&kJSClassDefinitionEmpty);
    ctx = JSGlobalContextCreate(global_class);
    auto global_object = JSContextGetGlobalObject(ctx);
    JSObjectSetPrivate(global_object, static_cast<void*>(this));

    add_function("log", &log);

    auto desktop_app_class = desktop_app_create_class();
    desktop_app_index = ObjectsIndex<DesktopApp>(ctx, desktop_app_class);
    add_constructor("DesktopApp", desktop_app_class,
                    desktop_app_call_as_constructor);

    auto desktop_window_class = desktop_window_create_class();
    desktop_window_index =
        ObjectsIndex<DesktopWindow>(ctx, desktop_window_class);
    add_constructor("DesktopWindow", desktop_window_class, NULL);
}

BindingsHost::~BindingsHost() {
    JSGlobalContextRelease(ctx);
}

void BindingsHost::add_function(const char* name,
                                  JSObjectCallAsFunctionCallback function,
                                  JSPropertyAttributes attributes) {
    auto js_name = JSStringCreateWithUTF8CString(name);
    auto js_function =
        JSObjectMakeFunctionWithCallback(ctx,      // ctx
                                         js_name,  // name
                                         function  // callAsFunction
        );
    JSObjectSetProperty(ctx,                            // ctx
                        JSContextGetGlobalObject(ctx),  // object
                        js_name,                        // propertyName
                        js_function,                    // value
                        attributes,                     // attributes
                        nullptr                         // exception
    );
}

void BindingsHost::add_object(const char* name, JSObjectRef object,
                                JSPropertyAttributes attributes) {
    JSObjectSetProperty(ctx,                                  // ctx
                        JSContextGetGlobalObject(ctx),        // object
                        JSStringCreateWithUTF8CString(name),  // propertyName
                        object,                               // value
                        attributes,                           // attributes
                        nullptr                               // exception
    );
};

void BindingsHost::add_constructor(
    const char* name, JSClassRef jsclass,
    JSObjectCallAsConstructorCallback call_as_constructor) {
    auto constructor =
        JSObjectMakeConstructor(ctx, jsclass, call_as_constructor);
    add_object(name, constructor);
}

}  // namespace aardvark::js
