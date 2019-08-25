#pragma once

#include "JavaScriptCore/JavaScript.h"
#include "nod/nod.hpp"

namespace aardvark::js {

JSClassRef signal_connection_create_class();

JSValueRef signal_connection_to_js(JSContextRef ctx,
                                   nod::connection connection);
}
