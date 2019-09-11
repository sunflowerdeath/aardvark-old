#pragma once

#include "JavaScriptCore/JavaScript.h"

#define ADV_ELEM_JS_CLASS(name)                                           \
    JSClassRef name##_elem_create_class(JSClassRef base_class);           \
    JSObjectRef name##_elem_call_as_constructor(                          \
        JSContextRef ctx, JSObjectRef constructor, size_t argument_count, \
        const JSValueRef arguments[], JSValueRef* exception);

namespace aardvark::js {

JSClassRef element_create_class();
JSObjectRef element_call_as_constructor(JSContextRef ctx,
                                        JSObjectRef constructor,
                                        size_t argument_count,
                                        const JSValueRef arguments[],
                                        JSValueRef* exception);

ADV_ELEM_JS_CLASS(align)
ADV_ELEM_JS_CLASS(background)
ADV_ELEM_JS_CLASS(center)
ADV_ELEM_JS_CLASS(intrinsic_height)
ADV_ELEM_JS_CLASS(responder)
ADV_ELEM_JS_CLASS(sized)
ADV_ELEM_JS_CLASS(stack)
ADV_ELEM_JS_CLASS(text)

}  // namespace aardvark::js
