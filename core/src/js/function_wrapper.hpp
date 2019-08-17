#pragma once

#include <vector>
#include <functional>

#include "JavaScriptCore/JavaScript.h"

namespace aardvark::js {

template <class RetValType, class... ArgsTypes>
class FunctionWrapper {
  public:
    using ArgsToJsType =
        std::function<std::vector<JSValueRef>(JSContextRef, ArgsTypes...)>;
    using RetValFromJsType =
        std::function<RetValType(JSContextRef, JSValueRef)>;

    FunctionWrapper(
        JSContextRef ctx, JSValueRef function,
        ArgsToJsType args_to_js =
            [](JSContextRef ctx) { return std::vector<JSValueRef>(); },
        RetValFromJsType ret_val_from_js = [](JSContextRef ctx,
                                              JSValueRef value) {})
        : function(function),
          args_to_js(args_to_js),
          ret_val_from_js(ret_val_from_js) {
        this->ctx = JSContextGetGlobalContext(ctx);
        JSValueProtect(ctx, function);
    }

    // Copy
    FunctionWrapper(const FunctionWrapper& wrapper) {
        ctx = wrapper.ctx;
        function = wrapper.function;
        args_to_js = wrapper.args_to_js;
        ret_val_from_js = wrapper.ret_val_from_js;
        JSValueProtect(ctx, function);
    }

    ~FunctionWrapper() {
        JSValueUnprotect(ctx, function);
    }

    RetValType operator()(ArgsTypes... args) {
        auto js_args = args_to_js(ctx, args...);
        auto object = JSValueToObject(ctx, function, nullptr);
        // Pin because during the call wrapper may be destroyed
        auto pin_ret_val_from_js = ret_val_from_js;
        auto pin_ctx = ctx;
        auto js_ret_val =
            JSObjectCallAsFunction(ctx,             // ctx
                                   object,          // object
                                   nullptr,         // thisObject
                                   js_args.size(),  // argumentCount
                                   js_args.data(),  // arguments[],
                                   nullptr          // exception
            );
        return pin_ret_val_from_js(pin_ctx, js_ret_val);
    }

  private:
    JSContextRef ctx;
    JSValueRef function;
    ArgsToJsType args_to_js;
    RetValFromJsType ret_val_from_js;
};

}
