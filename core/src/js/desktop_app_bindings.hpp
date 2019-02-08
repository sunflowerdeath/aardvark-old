#pragma once

#include "JavaScriptCore/JavaScript.h"

namespace aardvark::js {

JSClassRef desktop_app_create_class();

JSObjectRef desktop_app_call_as_constructor(JSContextRef ctx,
                                            JSObjectRef constructor,
                                            size_t argumentCount,
                                            const JSValueRef arguments[],
                                            JSValueRef* exception);

}  // namespace aardvark::js
