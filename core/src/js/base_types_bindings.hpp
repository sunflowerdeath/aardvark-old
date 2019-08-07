#pragma once

#include "JavaScriptCore/JavaScript.h"
#include "../base_types.hpp"
#include "../elements/align.hpp"
#include "../elements/size.hpp"

namespace aardvark::js {

Value value_from_js(JSContextRef ctx, JSValueRef object);
JSValueRef value_to_js(JSContextRef ctx, const Value& value);

elements::EdgeInsets alignment_from_js(JSContextRef ctx, JSObjectRef object);
JSObjectRef alignment_to_js(JSContextRef ctx,
                            const elements::EdgeInsets& alignment);

elements::ASize size_from_js(JSContextRef ctx, JSObjectRef object);
JSObjectRef size_to_js(JSContextRef ctx, const elements::ASize& size);

}  // namespace aardvark::js

