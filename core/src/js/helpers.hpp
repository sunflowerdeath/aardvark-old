#pragma once

#include <unordered_map>
#include "JavaScriptCore/JavaScript.h"

namespace aardvark::js {

class JsStringCache {
  public:
    static JSStringRef get(const std::string& str);
  private:
    JSStringRef get_string(const std::string& str);
    std::unordered_map<std::string, JSStringRef> strings;
};

template <class T, T (*from_js)(JSContextRef, JSValueRef)>
void map_prop_from_js(JSContextRef ctx, JSObjectRef object,
                       const char* prop_name, T* out) {
    auto prop_name_str = JsStringCache::get(prop_name);
    if (JSObjectHasProperty(ctx, object, prop_name_str)) {
        auto prop_value =
            JSObjectGetProperty(ctx, object, prop_name_str, nullptr);
        *out = from_js(ctx, prop_value);
    }
}

template <class T, JSValueRef (*to_js)(JSContextRef, const T&)>
void map_prop_to_js(JSContextRef ctx, JSObjectRef object, const char* prop_name,
                    const T& value) {
    JSObjectSetProperty(ctx, object, JsStringCache::get(prop_name),
                        to_js(ctx, value), kJSPropertyAttributeNone, nullptr);
}

int int_from_js(JSContextRef ctx, JSValueRef value);
JSValueRef int_to_js(JSContextRef ctx, const int& value);

}
