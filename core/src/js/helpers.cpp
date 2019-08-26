#include "helpers.hpp"

namespace aardvark::js {

int int_from_js(JSContextRef ctx, JSValueRef value) {
    return static_cast<int>(JSValueToNumber(ctx, value, nullptr));
}

JSValueRef int_to_js(JSContextRef ctx, const int& value) {
    return JSValueMakeNumber(ctx, static_cast<int>(value));
}

std::string str_from_js(JSStringRef jsstring) {
    auto size = JSStringGetMaximumUTF8CStringSize(jsstring);
    auto buffer = new char[size];
    JSStringGetUTF8CString(jsstring, buffer, size);
    auto stdstring = std::string(buffer);
    delete[] buffer;
    return stdstring;
}

std::string str_from_js(JSContextRef ctx, JSValueRef value) {
    auto js_str = JSValueToStringCopy(ctx, value, nullptr);
    auto str = str_from_js(js_str);
    JSStringRelease(js_str);
    return str;
}

JSValueRef str_to_js(JSContextRef ctx, const std::string& str) {
    auto js_str = JSStringCreateWithUTF8CString(str.c_str());
    auto value = JSValueMakeString(ctx, js_str);
    JSStringRelease(js_str);
    return value;
};

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
