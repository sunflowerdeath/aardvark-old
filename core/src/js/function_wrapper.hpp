
#include <functional>
#include <vector>

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
        JSContextRef ctx, JSValueRef value,
        ArgsToJsType args_to_js =
            [](JSContextRef ctx) { return std::vector<JSValueRef>(); },
        RetValFromJsType ret_val_from_js = [](JSContextRef ctx,
                                              JSValueRef value) {})
        : value(value),
          args_to_js(args_to_js),
          ret_val_from_js(ret_val_from_js) {
        this->ctx = JSContextGetGlobalContext(ctx);
        JSGlobalContextRetain(this->ctx);
        JSValueProtect(ctx, value);
    }

    // Copy
    FunctionWrapper(const FunctionWrapper& wrapper) {
        ctx = wrapper.ctx;
        value = wrapper.value;
        args_to_js = wrapper.args_to_js;
        ret_val_from_js = wrapper.ret_val_from_js;
        JSGlobalContextRetain(ctx);
        JSValueProtect(ctx, value);
    }

    ~FunctionWrapper() { 
        JSValueUnprotect(ctx, value);
        JSGlobalContextRelease(ctx);
    }

    RetValType operator()(ArgsTypes... args) {
        auto object = JSValueToObject(ctx, value, nullptr);
        auto js_args = args_to_js(ctx, args...);
        // Pin because during the call wrapper may be destroyed
        auto pin_ret_val_from_js = ret_val_from_js;
        auto pin_ctx = ctx;
        auto exception = JSValueRef();
        auto js_ret_val =
            JSObjectCallAsFunction(ctx,             // ctx
                                   object,          // object
                                   nullptr,         // thisObject
                                   js_args.size(),  // argumentCount
                                   js_args.data(),  // arguments[],
                                   &exception        // exception
            );
        if (exception != nullptr)
            std::cout << "Exception in callback" << std::endl;
        // TODO check exception and stop event loop or call callback?
        return pin_ret_val_from_js(pin_ctx, js_ret_val);
    }

  private:
    JSGlobalContextRef ctx;
    JSValueRef value;
    ArgsToJsType args_to_js;
    RetValFromJsType ret_val_from_js;
};

}  // namespace aardvark::js
