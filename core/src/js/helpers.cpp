#include "helpers.hpp"

namespace aardvark::js {

int int_from_js(JSContextRef ctx, JSValueRef value) {
    return static_cast<int>(JSValueToNumber(ctx, value, nullptr));
}

JSValueRef int_to_js(JSContextRef ctx, const int& value) {
    return JSValueMakeNumber(ctx, static_cast<int>(value));
}

JSStringRef JsStringCache::get_string(const std::string& str) {
    auto it = strings.find(str);
    if (it != strings.end()) {
        return it->second;
    } else {
        auto res =
            strings.emplace(str, JSStringCreateWithUTF8CString(str.c_str()));
        return res.first->second;
    }
}

JSStringRef JsStringCache::get(const std::string& str) {
    static JsStringCache instance = JsStringCache();
    return instance.get_string(str);
}

}
