#pragma once

#include <functional>
#include <vector>

#include "JavaScriptCore/JavaScript.h"

namespace aardvark::js {

template <class RetValType, class... ArgsTypes>
class FunctionWrapper {
  public:
    using ArgsToJsType =
        std::function<std::vector<JSValueRef>(JSContextRef, ArgsTypes...)>;
    // returned JSValue might be nullptr when exception happened
    using RetValFromJsType =
        std::function<RetValType(JSContextRef, JSValueRef)>;
    using ExceptionHandler = std::function<void(JSContextRef, JSValueRef)>;

    FunctionWrapper(std::weak_ptr<JSGlobalContextWrapper> ctx, JSValueRef value,
                    ArgsToJsType args_to_js = nullptr,
                    RetValFromJsType ret_val_from_js = nullptr,
                    ExceptionHandler exception_handler = nullptr)
        : ctx(ctx),
          value(value),
          args_to_js(args_to_js),
          ret_val_from_js(ret_val_from_js),
          exception_handler(exception_handler) {
        if (auto ctx_lock = ctx.lock()) JSValueProtect(ctx_lock->ctx, value);
    }

    // Copy
    FunctionWrapper(const FunctionWrapper& wrapper) {
        ctx = wrapper.ctx;
        value = wrapper.value;
        args_to_js = wrapper.args_to_js;
        ret_val_from_js = wrapper.ret_val_from_js;
        exception_handler = wrapper.exception_handler;
        if (auto ctx_lock = ctx.lock()) JSValueProtect(ctx_lock->ctx, value);
    }

    ~FunctionWrapper() { 
        if (auto ctx_lock = ctx.lock()) JSValueUnprotect(ctx_lock->ctx, value);
    }

    RetValType operator()(ArgsTypes... args) {
        auto ctx_lock = ctx.lock();
        if (!ctx_lock) return ret_val_from_js(nullptr, nullptr);
        auto object = JSValueToObject(ctx_lock->ctx, value, nullptr);
        auto js_args = args_to_js ? args_to_js(ctx_lock->ctx, args...)
                                  : std::vector<JSValueRef>();
        // Pin because during the call wrapper may be destroyed
        auto pin_ret_val_from_js = ret_val_from_js;
        auto exception = JSValueRef();
        auto js_ret_val =
            JSObjectCallAsFunction(ctx_lock->ctx,   // ctx
                                   object,          // object
                                   nullptr,         // thisObject
                                   js_args.size(),  // argumentCount
                                   js_args.data(),  // arguments[],
                                   &exception       // exception
            );
        if (exception != nullptr && exception_handler) {
            exception_handler(ctx_lock->ctx, exception);
        }
        if (pin_ret_val_from_js) {
            return pin_ret_val_from_js(
                ctx_lock->ctx, exception == nullptr ? js_ret_val : nullptr);
        }  // else return type should be void
    }

  private:
    std::weak_ptr<JSGlobalContextWrapper> ctx;
    JSValueRef value;
    ArgsToJsType args_to_js;
    RetValFromJsType ret_val_from_js;
    ExceptionHandler exception_handler;
};

}  // namespace aardvark::js
