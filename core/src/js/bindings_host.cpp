#include "bindings_host.hpp"

#include <iostream>
#include <typeinfo>
#include <functional>

#include "../elements/elements.hpp"
#include "desktop_app_bindings.hpp"
#include "desktop_window_bindings.hpp"
#include "document_bindings.hpp"
#include "elements_bindings.hpp"
#include "signal_connection_bindings.hpp"

namespace aardvark::js {

// log

std::string jsstring_to_std(JSStringRef jsstring) {
    auto size = JSStringGetMaximumUTF8CStringSize(jsstring);
    auto buffer = new char[size];
    JSStringGetUTF8CString(jsstring, buffer, size);
    auto stdstring = std::string(buffer);
    delete[] buffer;
    return stdstring;
}

JSValueRef log(
    JSContextRef ctx, JSObjectRef function, JSObjectRef this_object,
    size_t argument_count, const JSValueRef arguments[], JSValueRef* exception
) {
    for (auto i = 0; i < argument_count; i++) {
        if (i != 0) std::cout <<  " ";
        auto str = JSValueToStringCopy(ctx, arguments[i], nullptr);
        std::cout << jsstring_to_std(str);
        JSStringRelease(str);
    }
    std::cout << std::endl;
    return JSValueMakeUndefined(ctx);
}

JSValueRef set_timeout(JSContextRef ctx, JSObjectRef function,
                       JSObjectRef this_object, size_t argument_count,
                       const JSValueRef arguments[], JSValueRef* exception) {
    auto id = BindingsHost::get(ctx)->timers_host.set_timeout(
        arguments[0], JSValueToNumber(ctx, arguments[1], nullptr));
    return JSValueMakeNumber(ctx, id);
}

JSValueRef clear_timeout(JSContextRef ctx, JSObjectRef function,
                         JSObjectRef this_object, size_t argument_count,
                         const JSValueRef arguments[], JSValueRef* exception) {
    BindingsHost::get(ctx)->timers_host.clear_timeout(
        JSValueToNumber(ctx, arguments[0], nullptr));
    return JSValueMakeUndefined(ctx);
}

// BindingsHost

BindingsHost::BindingsHost() {
    // Create empty dummy class because default object class does not allocate 
    // storage for private data
    auto global_class = JSClassCreate(&kJSClassDefinitionEmpty);
    ctx = JSGlobalContextCreate(global_class);
    timers_host.ctx = ctx;
    
    // Store pointer to the host in private data of the global object
    auto global_object = JSContextGetGlobalObject(ctx);
    JSObjectSetPrivate(global_object, static_cast<void*>(this));

    add_function("log", &log);
    add_function("setTimeout", &set_timeout);
    add_function("clearTimeout", &clear_timeout);

    auto desktop_app_class = desktop_app_create_class();
    desktop_app_index = ObjectsIndex<DesktopApp>(ctx, desktop_app_class);
    add_constructor("DesktopApp", desktop_app_class,
                    desktop_app_call_as_constructor);

    auto desktop_app_window_list_class = desktop_app_window_list_create_class();
    desktop_app_window_list_index =
        ObjectsIndex<DesktopApp>(ctx, desktop_app_window_list_class);

    auto desktop_window_class = desktop_window_create_class();
    desktop_window_index =
        ObjectsIndex<DesktopWindow>(ctx, desktop_window_class);
    add_constructor("DesktopWindow", desktop_window_class, nullptr);

    auto document_class = document_create_class();
    document_index = ObjectsIndex<Document>(ctx, document_class);
    add_constructor("Document", document_class, nullptr);

    auto signal_connection_class = signal_connection_create_class();
    signal_connection_index =
        ObjectsIndex<nod::connection>(ctx, signal_connection_class);

    element_class = element_create_class();
    element_index = ObjectsIndex<Element>(ctx, [this](Element* elem) {
        return this->get_element_js_class(elem);
    });

    register_elem_class("Align", typeid(elements::Align),
                        align_elem_create_class,
                        align_elem_call_as_constructor);

    register_elem_class("Background", typeid(elements::Background),
                        background_elem_create_class,
                        background_elem_call_as_constructor);

    register_elem_class("Responder", typeid(elements::ResponderElement),
                        responder_elem_create_class,
                        responder_elem_call_as_constructor);

    register_elem_class("Stack", typeid(elements::Stack),
                        stack_elem_create_class,
                        stack_elem_call_as_constructor);

    register_elem_class("Sized", typeid(elements::Sized),
                        sized_elem_create_class,
                        sized_elem_call_as_constructor);

    register_elem_class("Text", typeid(elements::Text), text_elem_create_class,
                        text_elem_call_as_constructor);
}

void BindingsHost::register_elem_class(
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

BindingsHost::~BindingsHost() {
    JSGlobalContextRelease(ctx);
}

BindingsHost* BindingsHost::get(JSContextRef ctx) {
    auto global_object = JSContextGetGlobalObject(ctx);
    return static_cast<BindingsHost*>(JSObjectGetPrivate(global_object));
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
}

void BindingsHost::add_constructor(
    const char* name, JSClassRef jsclass,
    JSObjectCallAsConstructorCallback call_as_constructor) {
    auto constructor =
        JSObjectMakeConstructor(ctx, jsclass, call_as_constructor);
    add_object(name, constructor);
}

}  // namespace aardvark::js
