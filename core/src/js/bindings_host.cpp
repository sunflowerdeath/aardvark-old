#include "bindings_host.hpp"
#include <typeinfo>
#include <functional>
#include "desktop_app_bindings.hpp"
#include "desktop_window_bindings.hpp"
#include "document_bindings.hpp"
#include "elements_bindings.hpp"
#include "../elements/elements.hpp"

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
    add_constructor("DesktopWindow", desktop_window_class, nullptr);

    auto document_class = document_create_class();
    document_index = ObjectsIndex<Document>(ctx, document_class);
    add_constructor("Document", document_class, nullptr);

    auto element_class = element_create_class();
    element_index = ObjectsIndex<Element>(ctx, [this](Element* elem) {
        return this->get_element_js_class(elem);
    });
    align_element_class = align_element_create_class(element_class);
    add_constructor("Align", align_element_class,
                    align_element_call_as_constructor);
    background_element_class = background_element_create_class(element_class);
    add_constructor("Background", background_element_class,
                    background_element_call_as_constructor);
}

JSClassRef BindingsHost::get_element_js_class(Element* elem) {
    auto& id = typeid(elem);
    if (id == typeid(elements::Align)) return align_element_class;
    if (id == typeid(elements::Background)) return background_element_class;
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
