#pragma once

#include "JavaScriptCore/JavaScript.h"
#include "../base_types.hpp"
#include "../elements/align.hpp"

namespace aardvark::js {

Value value_from_js(JSContextRef ctx, JSObjectRef object);

elements::EdgeInsets alignment_from_js(JSContextRef ctx, JSObjectRef object);

}  // namespace aardvark::js

