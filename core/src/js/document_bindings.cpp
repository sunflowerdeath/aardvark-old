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

JSValueRef pointer_event_to_js(JSContextRef ctx, PointerEvent event) {
    return JSValueMakeNumber(ctx, event.left);
}

std::vector<JSValueRef> pointer_event_handler_args_to_js(JSContextRef ctx,
                                                         PointerEvent event) {
    return std::vector<JSValueRef>{pointer_event_to_js(ctx, event)};
}

void js_value_to_void(JSContextRef ctx, JSValueRef value){};

template <class ReturnType, class... ArgsTypes>
class JsFunctionWrapper {
  public:
    using args_type =
        std::function<std::vector<JSValueRef>(JSContextRef, ArgsTypes...)>;
    using ret_type = std::function<ReturnType(JSContextRef, JSValueRef)>;

    JsFunctionWrapper(
        JSContextRef ctx, JSValueRef value,
        args_type args_to_js,
        ret_type ret_val_from_js)
        : ctx(ctx),
          value(value),
          args_to_js(args_to_js),
          ret_val_from_js(ret_val_from_js) {
        JSValueProtect(ctx, value);
    }

    JsFunctionWrapper(const JsFunctionWrapper& wrapper) {
        ctx = wrapper.ctx;
        value = wrapper.value;
        args_to_js = wrapper.args_to_js;
        ret_val_from_js = wrapper.ret_val_from_js;
        JSValueProtect(ctx, value);
    }

    ~JsFunctionWrapper() {
        JSValueUnprotect(ctx, value);
    }

    ReturnType operator()(ArgsTypes... args) {
        auto js_args = args_to_js(ctx, args...);
        auto object = JSValueToObject(ctx, value, nullptr);
        auto result = JSObjectCallAsFunction(ctx,             // ctx
                                             object,          // object
                                             nullptr,         // thisObject
                                             js_args.size(),  // argumentCount
                                             js_args.data(),  // arguments[],
                                             nullptr          // exception
        );
        return ret_val_from_js(ctx, result);
    }

  private:
    JSContextRef ctx;
    JSValueRef value;
    args_type args_to_js;
    ret_type ret_val_from_js;
};

JSValueRef document_add_handler(JSContextRef ctx, JSObjectRef function,
                                JSObjectRef object, size_t argument_count,
                                const JSValueRef arguments[],
                                JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto document = host->document_index->get_native_object(object);

    JSValueProtect(ctx, arguments[0]);
    auto handler = JsFunctionWrapper<void, PointerEvent>(
        JSContextGetGlobalContext(ctx),    // ctx
        arguments[0],                      // function
        pointer_event_handler_args_to_js,  // args_wrapper
        js_value_to_void                   // return_type_wrapper
    );
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
