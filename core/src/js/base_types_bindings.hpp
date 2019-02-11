#pragma once

#include "JavaScriptCore/JavaScript.h"
#include "../base_types.hpp"
#include "../elements/align.hpp"

namespace aardvark::js {

Value value_from_js(JSContextRef ctx, JSObjectRef object);

JSObjectRef value_to_js(JSContextRef ctx, const Value& value);

elements::EdgeInsets alignment_from_js(JSContextRef ctx, JSObjectRef object);

JSObjectRef alignment_to_js(JSContextRef ctx,
                            const elements::EdgeInsets& alignment);

}  // namespace aardvark::js

