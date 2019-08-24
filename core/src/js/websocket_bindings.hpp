#pragma once

#include "JavaScriptCore/JavaScript.h"

namespace aardvark::js {

JSClassRef websocket_create_class();

JSObjectRef websocket_call_as_constructor(JSContextRef ctx,
                                          JSObjectRef constructor,
                                          size_t argumentCount,
                                          const JSValueRef arguments[],
                                          JSValueRef* exception);

}  // namespace aardvark::js
