#pragma once

#include <unicode/unistr.h>

#include <memory>
#include <string>
#include <unordered_map>

#include "../utils/log.hpp"
#include "JavaScriptCore/JavaScript.h"

namespace aardvark::js {

// numbers

int int_from_js(JSContextRef ctx, JSValueRef value);
JSValueRef int_to_js(JSContextRef ctx, const int& value);

float float_from_js(JSContextRef ctx, JSValueRef value);
JSValueRef float_to_js(JSContextRef ctx, const float& value);

// strings

std::string str_from_js_str(JSStringRef js_str);

std::string str_from_js(JSContextRef ctx, JSValueRef value);
JSValueRef str_to_js(JSContextRef ctx, const std::string& str);

UnicodeString icu_str_from_js(JSContextRef ctx, JSValueRef value);
JSValueRef icu_str_to_js(JSContextRef ctx, const UnicodeString& str);

// wrappers

class JsGlobalContextWrapper
    : public std::enable_shared_from_this<JsGlobalContextWrapper> {
  public:
    JsGlobalContextWrapper() {
        // Create empty dummy class because default object class does not
        // allocate storage for private data
        auto global_class = JSClassCreate(&kJSClassDefinitionEmpty);
        ctx = JSGlobalContextCreate(global_class);
        auto global_object = JSContextGetGlobalObject(ctx);
        JSObjectSetPrivate(global_object, static_cast<void*>(this));
    }

    ~JsGlobalContextWrapper() { JSGlobalContextRelease(ctx); };

    JSGlobalContextRef get() { return ctx; };

    void* data;

    static std::shared_ptr<JsGlobalContextWrapper> make() {
        return std::make_shared<JsGlobalContextWrapper>();
    }

    static std::shared_ptr<JsGlobalContextWrapper> get(JSContextRef ctx) {
        auto global_object = JSContextGetGlobalObject(ctx);
        return static_cast<JsGlobalContextWrapper*>(
                   JSObjectGetPrivate(global_object))
            ->shared_from_this();
    }

  private:
    JSGlobalContextRef ctx;
};

class JsValueWrapper {
  public:
    JsValueWrapper() {}

    JsValueWrapper(std::weak_ptr<JsGlobalContextWrapper> ctx_wptr,
                   JSValueRef value)
        : ctx_wptr(ctx_wptr), value(value) {
        auto ctx_sptr = ctx_wptr.lock();
        JSValueProtect(ctx_sptr->get(), value);
    }

    ~JsValueWrapper() {
        auto ctx_sptr = ctx_wptr.lock();
        if (ctx_sptr) JSValueUnprotect(ctx_sptr->get(), value);
    }

    // copy
    JsValueWrapper(const JsValueWrapper& other) {
        ctx_wptr = other.ctx_wptr;
        value = other.value;
        if (auto ctx_sptr = ctx_wptr.lock()) {
            JSValueProtect(ctx_sptr->get(), value);
        }
    }

    // assign
    JsValueWrapper& operator=(const JsValueWrapper& rhs) {
        ctx_wptr = rhs.ctx_wptr;
        value = rhs.value;
        if (auto ctx_sptr = ctx_wptr.lock()) {
            JSValueProtect(ctx_sptr->get(), value);
        }
        return *this;
    }

    JSValueRef get() const { return value; }

  private:
    std::weak_ptr<JsGlobalContextWrapper> ctx_wptr;
    JSValueRef value;
};

class JsStringWrapper {
  public:
    explicit JsStringWrapper(const std::string& str) {
        this->str = JSStringCreateWithUTF8CString(str.c_str());
    }

    JsStringWrapper(const char* str) {
        this->str = JSStringCreateWithUTF8CString(str);
    }

    // copy
    JsStringWrapper(const JsStringWrapper& other) {
        str = other.str;
        JSStringRetain(str);
    }

    // assign
    JsStringWrapper& operator=(const JsStringWrapper& rhs) {
        str = rhs.str;
        JSStringRetain(str);
        return *this;
    }

    ~JsStringWrapper() { JSStringRelease(str); }

    JSStringRef get() const { return str; }

  private:
    JSStringRef str;
};

class JsStringCache {
  public:
    static JSStringRef get(const std::string& str);

  private:
    JSStringRef get_string(const std::string& str);
    std::unordered_map<std::string, JSStringRef> strings;
};

// Map props

template <class T, T (*from_js)(JSContextRef, JSValueRef)>
void map_prop_from_js(JSContextRef ctx, JSObjectRef object,
                      const char* prop_name, T* out) {
    auto js_prop_name = JsStringCache::get(prop_name);
    if (JSObjectHasProperty(ctx, object, js_prop_name)) {
        auto prop_value =
            JSObjectGetProperty(ctx, object, js_prop_name, nullptr);
        *out = from_js(ctx, prop_value);
    }
}

template <class T, JSValueRef (*to_js)(JSContextRef, const T&)>
void map_prop_to_js(JSContextRef ctx, JSObjectRef object, const char* prop_name,
                    const T& value) {
    JSObjectSetProperty(ctx, object, JsStringCache::get(prop_name),
                        to_js(ctx, value), kJSPropertyAttributeNone, nullptr);
}

}  // namespace aardvark::js
