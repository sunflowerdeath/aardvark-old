#include "websocket_bindings.hpp"

#include "../utils/websocket.hpp"
#include "bindings_host.hpp"
#include "helpers.hpp"

namespace aardvark::js {

JSValueRef websocket_send(JSContextRef ctx, JSObjectRef function,
                          JSObjectRef object, size_t argument_count,
                          const JSValueRef arguments[], JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto ws = host->websocket_index->get_native_object(object);
    auto js_str = JSValueToStringCopy(ctx, arguments[0], nullptr);
    ws->send(str_from_js(js_str));
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

JSValueRef websocket_get_state(JSContextRef ctx, JSObjectRef object,
                               JSStringRef property_name,
                               JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto ws = host->websocket_index->get_native_object(object);
    auto state =
        static_cast<std::underlying_type_t<WebsocketState>>(ws->state);
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

JSObjectRef get_prop(JSContextRef ctx, JSObjectRef object,
                     JSStringRef prop_name) {
    if (!JSObjectHasProperty(ctx, object, prop_name)) return nullptr;
    auto value = JSObjectGetProperty(ctx, object, prop_name, nullptr);
    if (JSValueIsObject(ctx, value)) {
        return JSValueToObject(ctx, value, nullptr);
    } else {
        return nullptr;
    }
}

// void websocket_init_signal(
    // JSContextRef ctx,
    // JSObjectRef ws,
    // nod::signal<void()> signal,
    // prop_name) {
// }

JSObjectRef websocket_call_as_constructor(JSContextRef ctx,
                                          JSObjectRef constructor,
                                          size_t argumentCount,
                                          const JSValueRef arguments[],
                                          JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto ws = std::make_shared<aardvark::Websocket>(host->event_loop->io,
                                                    "echo.websocket.org", "80");
    auto object = host->websocket_index->create_js_object(ws);
    auto global_ctx = JSContextGetGlobalContext(ctx);

    ws->start_signal.connect([ws = ws.get(), global_ctx, object]() {
        auto func = get_prop(global_ctx, object, JsStringCache::get("onstart"));
        if (func == nullptr) return;
        JSObjectCallAsFunction(global_ctx,  // ctx
                               func,        // object
                               nullptr,     // thisObject
                               0,           // argumentCount
                               nullptr,     // arguments[],
                               nullptr      // exception
        );
        // TODO handle exception
    });

    ws->close_signal.connect([ws = ws.get(), global_ctx, object]() {
        auto func = get_prop(global_ctx, object, JsStringCache::get("onclose"));
        if (func == nullptr) return;
        JSObjectCallAsFunction(global_ctx,  // ctx
                               func,        // object
                               nullptr,     // thisObject
                               0,           // argumentCount
                               nullptr,     // arguments[],
                               nullptr      // exception
        );
        // TODO handle exception
    });

    ws->error_signal.connect([ws = ws.get(), global_ctx,
                              object](std::string error) {
        auto func = get_prop(global_ctx, object, JsStringCache::get("onerror"));
        if (func == nullptr) return;
        JSObjectCallAsFunction(global_ctx,  // ctx
                               func,        // object
                               nullptr,     // thisObject
                               0,           // argumentCount
                               nullptr,     // arguments[],
                               nullptr      // exception
        );
        // TODO handle exception
    });
    /*
    ws.error_signal.connect([ws](){
    })
    ws.message_signal.connect([ws](){
    })
    ws.close_signal.connect([ws](){
    })
    */
    ws->start();
    return object;
};

}  // namespace aardvark::js
