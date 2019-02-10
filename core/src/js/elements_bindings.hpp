#pragma once

#include "JavaScriptCore/JavaScript.h"

namespace aardvark::js {

JSClassRef element_create_class();

JSClassRef background_element_create_class(JSClassRef element_class);

JSObjectRef background_element_call_as_constructor(JSContextRef ctx,
                                                   JSObjectRef constructor,
                                                   size_t argument_count,
                                                   const JSValueRef arguments[],
                                                   JSValueRef* exception);

JSClassRef align_element_create_class(JSClassRef element_class);

JSObjectRef align_element_call_as_constructor(JSContextRef ctx,
                                              JSObjectRef constructor,
                                              size_t argumentCount,
                                              const JSValueRef arguments[],
                                              JSValueRef* exception);

JSClassRef stack_element_create_class(JSClassRef element_class);

JSObjectRef stack_element_call_as_constructor(JSContextRef ctx,
                                              JSObjectRef constructor,
                                              size_t argument_count,
                                              const JSValueRef arguments[],
                                              JSValueRef* exception);

}  // namespace aardvark::js
