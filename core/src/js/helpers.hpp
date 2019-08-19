#pragma once

#include "JavaScriptCore/JavaScript.h"

namespace aardvark::js {

template <class T, T (*from_js)(JSContextRef, JSValueRef)>
void map_prop_from_js(JSContextRef ctx, JSObjectRef object,
                      const char* prop_name, T* out) {
    auto prop_name_str = JSStringCreateWithUTF8CString(prop_name);
    if (JSObjectHasProperty(ctx, object, prop_name_str)) {
        auto prop_value =
            JSObjectGetProperty(ctx, object, prop_name_str, nullptr);
        *out = from_js(ctx, prop_value);
    }
    JSStringRelease(prop_name_str);
}

template <class T, JSValueRef (*to_js)(JSContextRef, const T&)>
void map_prop_to_js(JSContextRef ctx, JSObjectRef object, const char* prop_name,
                    const T& value) {
    auto prop_name_str = JSStringCreateWithUTF8CString(prop_name);
    JSObjectSetProperty(ctx, object, prop_name_str, to_js(ctx, value),
                        kJSPropertyAttributeNone, nullptr);
    JSStringRelease(prop_name_str);
}

int int_from_js(JSContextRef ctx, JSValueRef value);
JSValueRef int_to_js(JSContextRef ctx, const int& value);

}
