#pragma once

#include "JavaScriptCore/JavaScript.h"
#include "../events.hpp"

namespace aardvark::js {

JSValueRef pointer_event_to_js(JSContextRef ctx, const PointerEvent& event);
JSValueRef key_event_to_js(JSContextRef ctx, const KeyEvent& event);
JSValueRef scroll_event_to_js(JSContextRef ctx, const ScrollEvent& event);

}
