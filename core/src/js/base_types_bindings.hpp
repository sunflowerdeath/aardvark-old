#pragma once

#include "JavaScriptCore/JavaScript.h"

#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../elements/align.hpp"
#include "../elements/sized.hpp"
#include "../elements/padding.hpp"

namespace aardvark::js {

Size size_from_js(JSContextRef ctx, JSValueRef js_value);
JSValueRef size_to_js(JSContextRef ctx, const Size& color);

BoxConstraints box_constraints_from_js(JSContextRef ctx, JSValueRef js_value);
JSValueRef box_constraints_to_js(JSContextRef ctx, const BoxConstraints& color);

SkColor color_from_js(JSContextRef ctx, JSValueRef js_value);
JSValueRef color_to_js(JSContextRef ctx, const SkColor& color);

Value value_from_js(JSContextRef ctx, JSValueRef object);
JSValueRef value_to_js(JSContextRef ctx, const Value& value);

elements::EdgeInsets alignment_from_js(JSContextRef ctx, JSObjectRef object);
JSObjectRef alignment_to_js(JSContextRef ctx,
                            const elements::EdgeInsets& alignment);

Padding padding_from_js(JSContextRef ctx, JSValueRef object);
JSValueRef padding_to_js(JSContextRef ctx, const Padding& padding);

elements::SizeConstraints size_constraints_from_js(JSContextRef ctx,
                                                   JSObjectRef object);
JSObjectRef size_constraints_to_js(JSContextRef ctx,
                                   const elements::SizeConstraints& size);

}  // namespace aardvark::js

