#include "document_bindings.hpp"

#include "../document.hpp"
#include "bindings_host.hpp"
#include "signal_connection_bindings.hpp"
#include "function_wrapper.hpp"
#include "events_bindings.hpp"

namespace aardvark::js {

// helpers

std::vector<JSValueRef> pointer_event_handler_args_to_js(JSContextRef ctx,
                                                         PointerEvent event) {
    return std::vector<JSValueRef>{pointer_event_to_js(ctx, event)};
}

FunctionWrapper<void, PointerEvent> pointer_event_handler_from_js(
    JSContextRef ctx, JSValueRef value) {
    return FunctionWrapper<void, PointerEvent>(
        BindingsHost::get(ctx)->ctx,      // ctx
        value,                            // function
        pointer_event_handler_args_to_js  // args_to_js
    );
};

// Document bindings

void document_finalize(JSObjectRef object) {
    ObjectsIndex<Document>::remove(object);
}

JSValueRef document_get_root(JSContextRef ctx, JSObjectRef object,
                             JSStringRef property_name, JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto document = host->document_index->get_native_object(object);
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

JSValueRef document_add_handler(JSContextRef ctx, JSObjectRef function,
                                JSObjectRef object, size_t argument_count,
                                const JSValueRef arguments[],
                                JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto document = host->document_index->get_native_object(object);
    auto handler = pointer_event_handler_from_js(ctx, arguments[0]);
    auto after =
        argument_count > 1 ? JSValueToBoolean(ctx, arguments[1]) : false;
    auto connection =
        document->pointer_event_manager->add_handler(handler, after);
    return signal_connection_to_js(ctx, std::move(connection));
}

JSValueRef document_start_tracking_pointer(JSContextRef ctx,
                                           JSObjectRef function,
                                           JSObjectRef object,
                                           size_t argument_count,
                                           const JSValueRef arguments[],
                                           JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto document = host->document_index->get_native_object(object);
    auto pointer_id = JSValueToNumber(ctx, arguments[0], nullptr);
    auto handler = pointer_event_handler_from_js(ctx, arguments[1]);
    auto connection = document->pointer_event_manager->start_tracking_pointer(
        pointer_id, handler);
    return signal_connection_to_js(ctx, std::move(connection));
}

JSClassRef document_create_class() {
    auto definition = kJSClassDefinitionEmpty;
    JSStaticFunction static_functions[] = {
        {"addHandler", document_add_handler, PROP_ATTR_STATIC},
        {"startTrackingPointer", document_start_tracking_pointer,
         PROP_ATTR_STATIC},
        {0, 0, 0}};
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
