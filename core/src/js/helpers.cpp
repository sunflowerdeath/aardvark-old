#include "helpers.hpp"

namespace aardvark::js {

// Numbers

int int_from_js(JSContextRef ctx, JSValueRef value) {
    return static_cast<int>(JSValueToNumber(ctx, value, nullptr));
}

JSValueRef int_to_js(JSContextRef ctx, const int& value) {
    return JSValueMakeNumber(ctx, static_cast<int>(value));
}

// Strings

std::string str_from_js_str(JSStringRef js_str) {
    auto size = JSStringGetMaximumUTF8CStringSize(js_str);
    auto buffer = new char[size];
    JSStringGetUTF8CString(js_str, buffer, size);
    auto str = std::string(buffer);
    delete[] buffer;
    return str;
}

std::string str_from_js(JSContextRef ctx, JSValueRef value) {
    auto js_str = JSValueToStringCopy(ctx, value, nullptr);
    auto str = str_from_js_str(js_str);
    JSStringRelease(js_str);
    return str;
}

JSValueRef str_to_js(JSContextRef ctx, const std::string& str) {
    auto js_str = JSStringCreateWithUTF8CString(str.c_str());
    auto value = JSValueMakeString(ctx, js_str);
    JSStringRelease(js_str);
    return value;
};

UnicodeString icu_str_from_js(JSContextRef ctx, JSValueRef value) {
    auto js_str =
        JSValueToStringCopy(ctx, value, /* exception */ nullptr);
    auto icu_str = UnicodeString(JSStringGetCharactersPtr(js_str),
                                 JSStringGetLength(js_str));
    JSStringRelease(js_str);
    return icu_str;
};

JSValueRef icu_str_to_js(JSContextRef ctx, const UnicodeString& str) {
    auto js_str = JSStringCreateWithCharacters(str.getBuffer(), str.length());
    auto value = JSValueMakeString(ctx, js_str);
    JSStringRelease(js_str);
    return value;
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
