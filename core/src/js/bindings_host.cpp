#include "bindings_host.hpp"

#include <functional>
#include <iostream>
#include <typeinfo>

#include "../elements/elements.hpp"
#include "desktop_app_bindings.hpp"
#include "desktop_window_bindings.hpp"
#include "document_bindings.hpp"
#include "elements_bindings.hpp"
#include "function_wrapper.hpp"
#include "helpers.hpp"
#include "signal_connection_bindings.hpp"
#include "websocket_bindings.hpp"

namespace aardvark::js {

JSValueRef log(JSContextRef ctx, JSObjectRef function, JSObjectRef this_object,
               size_t argument_count, const JSValueRef arguments[],
               JSValueRef* exception) {
    for (auto i = 0; i < argument_count; i++) {
        if (i != 0) std::cout << " ";
        std::cout << str_from_js(ctx, arguments[i]);
    }
    std::cout << std::endl;
    return JSValueMakeUndefined(ctx);
}

JSValueRef set_timeout(JSContextRef ctx, JSObjectRef function,
                       JSObjectRef this_object, size_t argument_count,
                       const JSValueRef arguments[], JSValueRef* exception) {
    auto timeout = JSValueToNumber(ctx, arguments[1], nullptr) * 1000;
    auto host = BindingsHost::get(ctx);
    auto id = host->event_loop->set_timeout(
        FunctionWrapper<void>(host->ctx, arguments[0]), timeout);
    return JSValueMakeNumber(ctx, id);
}

JSValueRef clear_timeout(JSContextRef ctx, JSObjectRef function,
                         JSObjectRef this_object, size_t argument_count,
                         const JSValueRef arguments[], JSValueRef* exception) {
    BindingsHost::get(ctx)->event_loop->clear_timeout(
        JSValueToNumber(ctx, arguments[0], nullptr));
    return JSValueMakeUndefined(ctx);
}

JSValueRef gc(JSContextRef ctx, JSObjectRef function, JSObjectRef this_object,
              size_t argument_count, const JSValueRef arguments[],
              JSValueRef* exception) {
    JSGarbageCollect(ctx);
    return JSValueMakeUndefined(ctx);
}

void log_error(JsError error) {
    std::cout << std::endl;
    auto& loc = error.location;
    if (!loc.source_url.empty()) {
        std::cout << "File: " << loc.source_url << std::endl;
    }
    if (loc.line != -1) {
        std::cout << "Line: " << loc.line << ", column:" << loc.column
                  << std::endl;
    }
    std::cout << std::endl;
}

// BindingsHost

BindingsHost::BindingsHost() {
    // Create empty dummy class because default object class does not allocate
    // storage for private data
    auto global_class = JSClassCreate(&kJSClassDefinitionEmpty);
    ctx = std::make_shared<JSGlobalContextWrapper>(
        JSGlobalContextCreate(global_class));

    module_loader =
        std::make_unique<ModuleLoader>(event_loop.get(), ctx->ctx, true);
    module_loader->exception_handler = [this](JsError error) {
        log_error(error);
        stop();
    };

    // Store pointer to the host in private data of the global object
    auto global_object = JSContextGetGlobalObject(ctx->ctx);
    JSObjectSetPrivate(global_object, static_cast<void*>(this));

    add_object("window", global_object, PROP_ATTR_STATIC);

    app = std::make_shared<DesktopApp>(event_loop);
    auto desktop_app_class = desktop_app_create_class();
    auto app_object = JSObjectMake(ctx->ctx, desktop_app_class, nullptr);
    add_object("application", app_object, PROP_ATTR_STATIC);

    add_function("log", &log);
    add_function("setTimeout", &set_timeout);
    add_function("clearTimeout", &clear_timeout);
    add_function("gc", &gc);

    desktop_app_window_list_index.emplace(
        ctx->ctx, desktop_app_window_list_create_class());
    desktop_window_index.emplace(ctx->ctx, desktop_window_create_class());
    document_index.emplace(ctx->ctx, document_create_class());
    signal_connection_index.emplace(ctx->ctx, signal_connection_create_class());
    auto websocket_class = websocket_create_class();
    websocket_index.emplace(ctx->ctx, websocket_class);
    add_constructor("WebSocket", websocket_class,
                    websocket_call_as_constructor);

    element_class = element_create_class();
    element_index.emplace(ctx->ctx, [this](Element* elem) {
        return this->get_element_js_class(elem);
    });
    add_elem_class("Align", typeid(elements::Align), align_elem_create_class,
                   align_elem_call_as_constructor);
    add_elem_class("Background", typeid(elements::Background),
                   background_elem_create_class,
                   background_elem_call_as_constructor);
    add_elem_class("Center", typeid(elements::Center), center_elem_create_class,
                   center_elem_call_as_constructor);
    add_elem_class("Responder", typeid(elements::ResponderElement),
                   responder_elem_create_class,
                   responder_elem_call_as_constructor);
    add_elem_class("Stack", typeid(elements::Stack), stack_elem_create_class,
                   stack_elem_call_as_constructor);
    add_elem_class("Sized", typeid(elements::Sized), sized_elem_create_class,
                   sized_elem_call_as_constructor);
    add_elem_class("Text", typeid(elements::Text), text_elem_create_class,
                   text_elem_call_as_constructor);
}

BindingsHost::~BindingsHost() {
    stop();
    // JSC context is always destroyed first
    ctx.reset();
}

BindingsHost* BindingsHost::get(JSContextRef ctx) {
    auto global_object = JSContextGetGlobalObject(ctx);
    return static_cast<BindingsHost*>(JSObjectGetPrivate(global_object));
}

void BindingsHost::run() {
    if (is_running) return;
    is_running = true;
    app->run();
    event_loop->run();
}

void BindingsHost::stop() {
    if (!is_running) return;
    is_running = false;
    app->stop();
    event_loop->stop();
}

void BindingsHost::add_function(const char* name,
                                JSObjectCallAsFunctionCallback function,
                                JSPropertyAttributes attributes) {
    auto js_name = JSStringCreateWithUTF8CString(name);
    auto js_function =
        JSObjectMakeFunctionWithCallback(ctx->ctx,  // ctx
                                         js_name,   // name
                                         function   // callAsFunction
        );
    JSObjectSetProperty(ctx->ctx,                            // ctx
                        JSContextGetGlobalObject(ctx->ctx),  // object
                        js_name,                             // propertyName
                        js_function,                         // value
                        attributes,                          // attributes
                        nullptr                              // exception
    );
    JSStringRelease(js_name);
}

void BindingsHost::add_object(const char* prop_name, JSObjectRef object,
                              JSPropertyAttributes attributes) {
    auto js_prop_name = JSStringCreateWithUTF8CString(prop_name);
    JSObjectSetProperty(ctx->ctx,                            // ctx
                        JSContextGetGlobalObject(ctx->ctx),  // object
                        js_prop_name,                        // propertyName
                        object,                              // value
                        attributes,                          // attributes
                        nullptr                              // exception
    );
    JSStringRelease(js_prop_name);
}

void BindingsHost::add_constructor(
    const char* name, JSClassRef jsclass,
    JSObjectCallAsConstructorCallback call_as_constructor) {
    auto constructor =
        JSObjectMakeConstructor(ctx->ctx, jsclass, call_as_constructor);
    add_object(name, constructor);
}

void BindingsHost::add_elem_class(
    const char* name, const std::type_info& elem_type,
    JSCreateClassCallback create_class,
    JSObjectCallAsConstructorCallback call_as_constructor) {
    auto js_class = create_class(element_class);
    elements_classes[std::type_index(elem_type)] = js_class;
    add_constructor(name, js_class, call_as_constructor);
}

JSClassRef BindingsHost::get_element_js_class(Element* elem) {
    return elements_classes[std::type_index(typeid(*elem))];
}

}  // namespace aardvark::js
