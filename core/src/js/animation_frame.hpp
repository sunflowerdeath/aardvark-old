#pragma once

#include <map>
#include <functional>
#include "JavaScriptCore/JavaScript.h"
#include "bindings_host.hpp"
#include "function_wrapper.hpp"

namespace aardvark::js {

class BindingsHost;

class AnimationFrame {
  public:
    int add_callback(std::function<void()> callback) {
        id++;
        callbacks[id] = callback;
        return id;
    }

    void remove_callback(int id) {
        callbacks.erase(id);
    }

    void call_callbacks() {
        for (auto it : callbacks) it.second();
        callbacks.clear();
    }

  private:
    int id = 0;
    std::map<int, std::function<void()>> callbacks;
};

JSValueRef request_animation_frame(JSContextRef ctx, JSObjectRef function,
                                   JSObjectRef this_object,
                                   size_t argument_count,
                                   const JSValueRef arguments[],
                                   JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto id = host->animation_frame->add_callbacks(
        FunctionWrapper<void, PointerEvent>(
            host->ctx,     // ctx
            arguments[0],  // function
            nullptr,       // args_to_js
            nullptr,       // ret_val_from_js
            [host](JSContextRef ctx, JSValueRef exception) {
                host->module_loader->handle_exception(exception);
            }  // exception_handler
            ));
    return JSValueMakeNumber(ctx, id, nullptr);
}

JSValueRef cancel_animation_frame(JSContextRef ctx, JSObjectRef function,
                                  JSObjectRef this_object,
                                  size_t argument_count,
                                  const JSValueRef arguments[],
                                  JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto id = JSValueToNumber(ctx, arguments[0], nullptr);
    host->animation_frame->remove_callbacks(id);
    return JSValueMakeUndefined(ctx);
}

}  // namespace aardvark::js
