#include "helpers.hpp"

namespace aardvark::js {

int int_from_js(JSContextRef ctx, JSValueRef value) {
    return static_cast<int>(JSValueToNumber(ctx, value, nullptr));
}

JSValueRef int_to_js(JSContextRef ctx, const int& value) {
    return JSValueMakeNumber(ctx, static_cast<int>(value));
}

}
