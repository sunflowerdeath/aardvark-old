#include "signal_connection_bindings.hpp"

#include "bindings_host.hpp"
#include "objects_index.hpp"

namespace aardvark::js {

void signal_connection_finalize(JSObjectRef object) {
    ObjectsIndex<nod::connection>::remove(object);
}

JSValueRef signal_connection_call_as_function(JSContextRef ctx,
                                              JSObjectRef function,
                                              JSObjectRef this_object,
                                              size_t argument_count,
                                              const JSValueRef arguments[],
                                              JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto connection =
        host->signal_connection_index->get_native_object(function);
    connection->disconnect();
    return JSValueMakeUndefined(ctx);
}

JSClassRef signal_connection_create_class() {
    auto definition = kJSClassDefinitionEmpty;
    definition.callAsFunction = signal_connection_call_as_function;
    definition.className = "SignalConnection";
    definition.finalize = signal_connection_finalize;
    return JSClassCreate(&definition);
}

JSValueRef signal_connection_to_js(JSContextRef ctx,
                                   nod::connection connection) {
    auto host = BindingsHost::get(ctx);
    auto sptr = std::make_shared<nod::connection>(std::move(connection));
    return host->signal_connection_index->get_or_create_js_object(sptr);
}

// Connection

void connection_finalize(JSObjectRef object) {
    ObjectsIndex<Connection>::remove(object);
}

JSValueRef connection_call_as_function(JSContextRef ctx, JSObjectRef function,
                                       JSObjectRef this_object,
                                       size_t argument_count,
                                       const JSValueRef arguments[],
                                       JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto connection = host->connection_index->get_native_object(function);
    connection->disconnect();
    return JSValueMakeUndefined(ctx);
}

JSClassRef connection_create_class() {
    auto definition = kJSClassDefinitionEmpty;
    definition.callAsFunction = connection_call_as_function;
    definition.className = "Connection";
    definition.finalize = connection_finalize;
    return JSClassCreate(&definition);
}

JSValueRef connection_to_js(JSContextRef ctx, const Connection& connection) {
    auto host = BindingsHost::get(ctx);
    auto sptr = std::make_shared<Connection>(std::move(connection));
    return host->connection_index->get_or_create_js_object(sptr);
}

}  // namespace aardvark::js
