#include "websocket_bindings.hpp"

#include "../utils/websocket.hpp"
#include "bindings_host.hpp"
#include "function_wrapper.hpp"
#include "helpers.hpp"
#include "signal_connection_bindings.hpp"

namespace aardvark::js {

JSValueRef websocket_send(JSContextRef ctx, JSObjectRef function,
                          JSObjectRef object, size_t argument_count,
                          const JSValueRef arguments[], JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto ws = host->websocket_index->get_native_object(object);
    ws->send(str_from_js(ctx, arguments[0]));
    return JSValueMakeUndefined(ctx);
}

JSValueRef websocket_close(JSContextRef ctx, JSObjectRef function,
                           JSObjectRef object, size_t argument_count,
                           const JSValueRef arguments[],
                           JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto ws = host->websocket_index->get_native_object(object);
    ws->close();
    return JSValueMakeUndefined(ctx);
}

std::vector<JSValueRef> websocket_error_handler_args_to_js(
    JSContextRef ctx, const std::string& error) {
    auto object = JSObjectMake(ctx, nullptr, nullptr);
    map_prop_to_js<std::string, str_to_js>(ctx, object, "message", error);
    return std::vector<JSValueRef>{object};
}

std::vector<JSValueRef> websocket_message_handler_args_to_js(
    JSContextRef ctx, const std::string& message) {
    auto object = JSObjectMake(ctx, nullptr, nullptr);
    map_prop_to_js<std::string, str_to_js>(ctx, object, "data", message);
    return std::vector<JSValueRef>{object};
}

JSValueRef websocket_add_open_handler(JSContextRef ctx, JSObjectRef function,
                                      JSObjectRef object, size_t argument_count,
                                      const JSValueRef arguments[],
                                      JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto ws = host->websocket_index->get_native_object(object);
    auto connection =
        ws->open_signal.connect(FunctionWrapper<void>(host->ctx, arguments[0]));
    return signal_connection_to_js(ctx, std::move(connection));
}

JSValueRef websocket_add_message_handler(JSContextRef ctx, JSObjectRef function,
                                         JSObjectRef object,
                                         size_t argument_count,
                                         const JSValueRef arguments[],
                                         JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto ws = host->websocket_index->get_native_object(object);
    auto connection =
        ws->message_signal.connect(FunctionWrapper<void, std::string>(
            host->ctx, arguments[0], websocket_message_handler_args_to_js));
    return signal_connection_to_js(ctx, std::move(connection));
}

JSValueRef websocket_add_error_handler(JSContextRef ctx, JSObjectRef function,
                                       JSObjectRef object,
                                       size_t argument_count,
                                       const JSValueRef arguments[],
                                       JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto ws = host->websocket_index->get_native_object(object);
    auto connection =
        ws->error_signal.connect(FunctionWrapper<void, std::string>(
            host->ctx, arguments[0], websocket_error_handler_args_to_js));
    return signal_connection_to_js(ctx, std::move(connection));
}

JSValueRef websocket_add_close_handler(JSContextRef ctx, JSObjectRef function,
                                       JSObjectRef object,
                                       size_t argument_count,
                                       const JSValueRef arguments[],
                                       JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto ws = host->websocket_index->get_native_object(object);
    auto connection = ws->close_signal.connect(
        FunctionWrapper<void>(host->ctx, arguments[0]));
    return signal_connection_to_js(ctx, std::move(connection));
}

JSValueRef websocket_get_state(JSContextRef ctx, JSObjectRef object,
                               JSStringRef property_name,
                               JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto ws = host->websocket_index->get_native_object(object);
    auto state = static_cast<std::underlying_type_t<WebsocketState>>(ws->state);
    return JSValueMakeNumber(ctx, state);
}

void websocket_finalize(JSObjectRef object) {
    ObjectsIndex<Websocket>::remove(object);
}

JSClassRef websocket_create_class() {
    auto definition = kJSClassDefinitionEmpty;
    JSStaticFunction static_functions[] = {
        {"send", websocket_send, PROP_ATTR_STATIC},
        {"close", websocket_close, PROP_ATTR_STATIC},
        {"addOpenHandler", websocket_add_open_handler, PROP_ATTR_STATIC},
        {"addMessageHandler", websocket_add_message_handler, PROP_ATTR_STATIC},
        {"addErrorHandler", websocket_add_error_handler, PROP_ATTR_STATIC},
        {"addCloseHandler", websocket_add_close_handler, PROP_ATTR_STATIC},
        {0, 0, 0}};
    JSStaticValue static_values[] = {
        {"state", websocket_get_state, nullptr, PROP_ATTR_STATIC},
        {0, 0, 0, 0}};
    definition.className = "Websocket";
    definition.staticFunctions = static_functions;
    definition.staticValues = static_values;
    definition.finalize = websocket_finalize;
    return JSClassCreate(&definition);
};

JSObjectRef websocket_call_as_constructor(JSContextRef ctx,
                                          JSObjectRef constructor,
                                          size_t argumentCount,
                                          const JSValueRef arguments[],
                                          JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto ws = std::make_shared<aardvark::Websocket>(
        host->event_loop->io,            // io
        str_from_js(ctx, arguments[0]),  // host
        str_from_js(ctx, arguments[1])   // port
    );
    ws->open();
    return host->websocket_index->create_js_object(ws);
};

}  // namespace aardvark::js
