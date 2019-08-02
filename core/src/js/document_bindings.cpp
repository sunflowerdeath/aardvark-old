#include <stdio.h>
#include "document_bindings.hpp"
#include "bindings_host.hpp"
#include "../document.hpp"

namespace aardvark::js {

void document_finalize(JSObjectRef object) {
    ObjectsIndex<DesktopApp>::remove(object);
}

JSValueRef document_get_root(JSContextRef ctx, JSObjectRef object,
                             JSStringRef property_name, JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto document = host->document_index->get_native_object(object);
    // ?? it is placeholder but how to create it ??
    // some sort of RTTI???
    return host->element_index->get_or_create_js_object(document->root);
}

bool document_set_root(JSContextRef ctx, JSObjectRef object,
                       JSStringRef propertyName, JSValueRef value,
                       JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto document = host->document_index->get_native_object(object);
    auto elem = host->element_index->get_native_object(
        JSValueToObject(ctx, value, exception));
    document->set_root(elem);
    return true;
}

class JsFunctionWrapper {
  public:
    JsFunctionWrapper(JSContextRef ctx, JSValueRef value)
        : ctx(ctx), value(value) {
        JSValueProtect(ctx, value);
    }

    JsFunctionWrapper(const JsFunctionWrapper& wrapper) {
        ctx = wrapper.ctx;
        value = wrapper.value;
        JSValueProtect(ctx, value);
    }

    ~JsFunctionWrapper() {
        JSValueUnprotect(ctx, value);
    }

    void operator()(PointerEvent event) {
        auto object = JSValueToObject(ctx, value, nullptr);
        // TODO convert arguments
        JSObjectCallAsFunction(ctx,      // ctx
                               object,   // object
                               nullptr,  // thisObject
                               0,        // argumentCount
                               {},       // arguments[],
                               nullptr   // exception
        );
    }

  private:
    JSContextRef ctx;
    JSValueRef value;
};

JSValueRef document_add_handler(JSContextRef ctx, JSObjectRef function,
                                JSObjectRef object, size_t argument_count,
                                const JSValueRef arguments[],
                                JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto document = host->document_index->get_native_object(object);

    JSValueProtect(ctx, arguments[0]);
    auto handler =
        JsFunctionWrapper(JSContextGetGlobalContext(ctx), arguments[0]);
    document->pointer_event_manager->add_handler(handler);
    return JSValueMakeUndefined(ctx);
}

JSClassRef document_create_class() {
    auto definition = kJSClassDefinitionEmpty;
    JSStaticFunction static_functions[] = {
        {"addHandler", document_add_handler, PROP_ATTR_STATIC}, {0, 0, 0}};
    JSStaticValue static_values[] = {
        {"root", document_get_root, document_set_root,
         kJSPropertyAttributeNone},
        {0, 0, 0, 0}};
    definition.className = "Document";
    definition.staticFunctions = static_functions;
    definition.staticValues = static_values;
    definition.finalize = document_finalize;
    return JSClassCreate(&definition);
}

}
