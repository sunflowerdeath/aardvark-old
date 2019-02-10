#pragma once

#include "JavaScriptCore/JavaScript.h"

namespace aardvark::js {

JSClassRef desktop_app_create_class();

JSObjectRef desktop_app_call_as_constructor(JSContextRef ctx,
                                            JSObjectRef constructor,
                                            size_t argumentCount,
                                            const JSValueRef arguments[],
                                            JSValueRef* exception);

JSClassRef desktop_app_window_list_create_class();

}  // namespace aardvark::js
