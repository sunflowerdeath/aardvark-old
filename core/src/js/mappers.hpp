#pragma once

#include <functional>
#include <vector>
#include "JavaScriptCore/JavaScript.h"

// Helper for shorter defining PropMapper
#define ADV_PROP_MAPPER(T, F, MAPPER, PROP_NAME, MEMBER_NAME) \
    PropMapper<T, F>(&MAPPER, PROP_NAME,                      \
                     [](T* value) { return &(value->MEMBER_NAME); })

namespace aardvark::js {

// Interface for mapping native value to JS value
template <typename T>
class Mapper {
  public:
    virtual JSValueRef to_js(JSContextRef ctx, const T& value);
    virtual T from_js(JSContextRef ctx, JSValueRef value);
};

// Maps two types using provided FromJs and ToJS functions
template <typename T, JSValueRef (*ToJs)(JSContextRef, const T&),
          T (*FromJs)(JSContextRef, JSValueRef)>
class SimpleMapper : public Mapper<T> {
  public:
    JSValueRef to_js(JSContextRef ctx, const T& value) override {
        return ToJs(ctx, value);
    };
    T from_js(JSContextRef ctx, JSValueRef js_value) override {
        return FromJs(ctx, js_value);
    };
};

// This base class is needed to store prop mappers of different types
// in one container
template <typename T>
class BasePropMapper {
  public:
    virtual void map_to_js(JSContextRef ctx, JSObjectRef object,
                           const T* value){};
    virtual void map_from_js(JSContextRef ctx, JSObjectRef object, T* value){};
};

template <typename T, typename F>
class PropMapper : public BasePropMapper<T> {
  public:
    PropMapper(Mapper<F>* mapper, const char* prop_name,
               std::function<F*(T*)> getter)
        : mapper(mapper),
          getter(getter),
          prop_name(JsStringCache::get(prop_name)){};

    void map_to_js(JSContextRef ctx, JSObjectRef object,
                   const T* value) override {
        JSObjectSetProperty(ctx, object, prop_name,
                            mapper->to_js(ctx, *getter(const_cast<T*>(value))),
                            kJSPropertyAttributeNone, nullptr);
    };

    void map_from_js(JSContextRef ctx, JSObjectRef object, T* value) override {
        if (JSObjectHasProperty(ctx, object, prop_name)) {
            auto prop_value =
                JSObjectGetProperty(ctx, object, prop_name, nullptr);
            *(getter(value)) = mapper->from_js(ctx, prop_value);
        }
    };

  private:
    Mapper<F>* mapper;
    JSStringRef prop_name;
    std::function<F*(T*)> getter;
};

// Maps object properties to members of a native object using list of
// PropMappers
template <typename T>
class ObjectMapper : public Mapper<T> {
  public:
    ObjectMapper(std::vector<BasePropMapper<T>> fields) : fields(fields){};

    JSValueRef to_js(JSContextRef ctx, const T& value) override {
        auto object = JSObjectMake(ctx, nullptr, nullptr);
        for (auto& field : fields) field.map_to_js(ctx, object, &value);
        return object;
    };

    T from_js(JSContextRef ctx, JSValueRef js_value) override {
        auto object = JSValueToObject(ctx, js_value, nullptr);
        auto value = T();
        for (auto& field : fields) field.map_from_js(ctx, object, &value);
        return value;
    };

  private:
    std::vector<BasePropMapper<T>> fields;
};

}  // namespace aardvark::js

