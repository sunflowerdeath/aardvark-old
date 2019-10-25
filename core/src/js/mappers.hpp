#pragma once

#include <functional>
#include <memory>
#include <vector>
#include "JavaScriptCore/JavaScript.h"

namespace aardvark::js {

// Interface for mapping native value to JS value
template <typename T>
class Mapper {
  public:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
    virtual JSValueRef to_js(JSContextRef ctx, const T& value){};
    virtual T from_js(JSContextRef ctx, JSValueRef value){};
#pragma GCC diagnostic pop
};

template <typename T>
class EnumMapper : public Mapper<T> {
    using UnderlyingMapper = Mapper<std::underlying_type_t<T>>;

  public:
    EnumMapper(UnderlyingMapper* mapper) : mapper(mapper){};

    JSValueRef to_js(JSContextRef ctx, const T& value) override {
        return mapper->to_js(ctx,
                             static_cast<std::underlying_type_t<T>>(value));
    };

    T from_js(JSContextRef ctx, JSValueRef value) override {
        return static_cast<T>(mapper->from_js(ctx, value));
    };

  private:
    UnderlyingMapper* mapper;
};

// Maps two types using provided mapping functions
template <typename T, JSValueRef (*to_js_func)(JSContextRef, const T&),
          T (*from_js_func)(JSContextRef, JSValueRef)>
class SimpleMapper : public Mapper<T> {
  public:
    JSValueRef to_js(JSContextRef ctx, const T& value) override {
        return to_js_func(ctx, value);
    };
    T from_js(JSContextRef ctx, JSValueRef value) override {
        return from_js_func(ctx, value);
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
        : mapper(mapper), getter(getter), prop_name(prop_name){};

    void map_to_js(JSContextRef ctx, JSObjectRef object,
                   const T* value) override {
        auto js_prop_name = JsStringCache::get(prop_name);
        JSObjectSetProperty(ctx, object, js_prop_name,
                            mapper->to_js(ctx, *getter(const_cast<T*>(value))),
                            kJSPropertyAttributeNone, nullptr);
    };

    void map_from_js(JSContextRef ctx, JSObjectRef object, T* value) override {
        auto js_prop_name = JsStringCache::get(prop_name);
        if (JSObjectHasProperty(ctx, object, js_prop_name)) {
            auto prop_value =
                JSObjectGetProperty(ctx, object, js_prop_name, nullptr);
            *(getter(value)) = mapper->from_js(ctx, prop_value);
        }
    };

  private:
    Mapper<F>* mapper;
    const char* prop_name;
    std::function<F*(T*)> getter;
};

// Maps object properties to members of a native object using list of
// PropMappers
template <typename T>
class ObjectMapper : public Mapper<T> {
  public:
    ObjectMapper(std::vector<BasePropMapper<T>*> fields) : fields(fields){};

    JSValueRef to_js(JSContextRef ctx, const T& value) override {
        auto object = JSObjectMake(ctx, nullptr, nullptr);
        for (auto& field : fields) field->map_to_js(ctx, object, &value);
        return object;
    };

    T from_js(JSContextRef ctx, JSValueRef js_value) override {
        auto object = JSValueToObject(ctx, js_value, nullptr);
        T value;
        for (auto& field : fields) field->map_from_js(ctx, object, &value);
        return value;
    };

  private:
    std::vector<BasePropMapper<T>*> fields;
};

template <typename F, typename... Ts>
inline void template_foreach(F f, const Ts&... args) {
    // initializer_list allows to expand variadic template parameter `args` to
    // multiple statements. Each expansion calls provided lambda function with
    // item from `args`.
    (void)std::initializer_list<int>{[&f](const auto& arg) {
        f(arg);
        return 0;
    }(args)...};
}

template <typename T, typename... F>
class BetterObjectMapper : public Mapper<T> {
  public:
    BetterObjectMapper(std::tuple<const char*, F T::*, Mapper<F>*>... fields) {
        template_foreach(
            [&](const auto& arg) { prop_names.push_back(std::get<0>(arg)); });

        // Generated function that iterates over all property definitions and
        // maps js properties and corresponding object members using mappers
        map_props_to_js = [&](JSContextRef ctx, const T& value) {
            auto result = JSObjectMake(ctx, nullptr, nullptr);
            template_foreach(
                [&](const auto& def) {
                    auto [prop_name, member_ptr, mapper] = def;
                    auto prop_value = mapper->to_js(ctx, value.*member_ptr);
                    auto js_prop_name = JsStringCache::get(prop_name);
                    JSObjectSetProperty(ctx, result, js_prop_name, prop_value,
                                        kJSPropertyAttributeNone, nullptr);
                },
                fields...);
            return result;
        };

        map_props_from_js = [&](JSContextRef ctx, JSValueRef value) {
            auto object = JSValueToObject(ctx, value, nullptr);
            T result;
            template_foreach([&](const auto& def){
                auto [prop_name, member_ptr, mapper] = def;
                auto js_prop_name = JsStringCache::get(prop_name);
                if (JSObjectHasProperty(ctx, object, js_prop_name)) {
                    auto prop_value =
                        JSObjectGetProperty(ctx, object, js_prop_name, nullptr);
                    result.*member_ptr = mapper->from_js(ctx, prop_value);
                }
            }, fields...);
            return result;
        };
    }

    JSValueRef to_js(JSContextRef ctx, const T& value) override {
        return map_props_to_js(ctx, value);
    };

    T from_js(JSContextRef ctx, JSValueRef value) override {
        return map_props_from_js(ctx, value);
    };

  private:
    std::vector<const char*> prop_names;
    std::function<JSValueRef(JSContextRef ctx, const T& value)> map_props_to_js;
    std::function<T(JSContextRef ctx, JSValueRef value)> map_props_from_js;
};

}  // namespace aardvark::js

