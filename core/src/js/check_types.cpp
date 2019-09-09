#include "check_types.hpp"

#include "fmt/format.h"
#include <iostream>

namespace aardvark::js::check_types {

bool to_exception(const Checker& checker, JSContextRef ctx, JSValueRef value,
                  const std::string& name, const std::string& location,
                  JSValueRef* exception) {
    auto error = checker(ctx, value, name, location);
    if (error.has_value()) {
        auto obj = JSObjectMake(ctx, nullptr, nullptr);
        map_prop_to_js<std::string, str_to_js>(ctx, obj, "message",
                                               error.value());
        *exception = obj;
    }
}

std::string get_type(JSContextRef ctx, JSValueRef value) {
    auto type = JSValueGetType(ctx, value);
    if (type == kJSTypeUndefined) return "undefined";
    if (type == kJSTypeBoolean) return "boolean";
    if (type == kJSTypeNumber) return "number";
    if (type == kJSTypeString) return "string";
    if (type == kJSTypeObject) {
        // TODO null not object ?
        if (JSValueIsArray(ctx, value)) return "array";
        return "object";
    }
    if (type == kJSTypeSymbol) return "symbol";
    return "unknown";
}

Checker make_primitive(const std::string& expected_type) {
    return [expected_type](JSContextRef ctx, JSValueRef value,
                           const std::string& name,
                           const std::string& location) -> CheckResult {
        auto type = get_type(ctx, value);
        if (type == expected_type) return std::nullopt;
        return fmt::format(
            "Invalid property `{}` of type `{}` supplied to {}, "
            "expected `{}`.",
            name, type, location, expected_type);
    };
}

Checker number = make_primitive("number");
Checker boolean = make_primitive("boolean");
Checker object = make_primitive("object");
Checker array = make_primitive("array");
Checker string = make_primitive("string");
Checker symbol = make_primitive("symbol");

Checker array_of(Checker checker) {
    return
        [checker](JSContextRef ctx, JSValueRef value, const std::string& name,
                  const std::string& location) -> CheckResult {
            auto array_error = array(ctx, value, name, location);
            if (array_error.has_value()) return array_error;

            auto object = JSValueToObject(ctx, value, nullptr);
            auto length = 0;
            map_prop_from_js<int, int_from_js>(ctx, object, "length", &length);
            JSValueRef exception;
            for (auto i = 0; i < length; i++) {
                auto item =
                    JSObjectGetPropertyAtIndex(ctx, object, i, &exception);
                auto error = checker(
                    ctx, item, name + "[" + std::to_string(i) + "]", location);
                if (error.has_value()) return error;
            }

            return std::nullopt;
        };
}

Checker object_of(Checker checker) {
    return
        [checker](JSContextRef ctx, JSValueRef value, const std::string& name,
                  const std::string& location) -> CheckResult {
            auto object_error = object(ctx, value, name, location);
            if (object_error.has_value()) return object_error;

            auto object = JSValueToObject(ctx, value, nullptr);
            auto props_names = JSObjectCopyPropertyNames(ctx, object);
            auto props_count = JSPropertyNameArrayGetCount(props_names);
            CheckResult result = std::nullopt;
            for (auto i = 0; i < props_count; i++) {
                auto prop_name =
                    JSPropertyNameArrayGetNameAtIndex(props_names, i);
                auto prop_value =
                    JSObjectGetProperty(ctx, object, prop_name, nullptr);
                auto prop_result =
                    checker(ctx, prop_value,
                            name + "." + str_from_js_str(prop_name), location);
                if (prop_result.has_value()) {
                    result = prop_result;
                    break;
                }
            }
            JSPropertyNameArrayRelease(props_names);
            return result;
        };
}

Checker instance_of(JSClassRef cls) {
    return
        [cls](JSContextRef ctx, JSValueRef value, const std::string& name,
                  const std::string& location) -> CheckResult {
            auto object_error = object(ctx, value, name, location);
            if (object_error.has_value()) return object_error;
            if (JSValueIsObjectOfClass(ctx, value, cls)) return std::nullopt;
            // TODO cls names
            auto actual_class_name = "ACTUAL_CLASS_NAME";
            auto expected_class_name = "EXPECTED_CLASS_NAME";
            return fmt::format(
                "Invalid property `{}` of type `{}` supplied to `{}`, expected "
                "instance of `{}`.",
                name, actual_class_name, location, expected_class_name);
        };
}

Checker make_union(std::initializer_list<Checker> checkers) {
    return [checkers](JSContextRef ctx, JSValueRef value,
                      const std::string& name,
                      const std::string& location) -> CheckResult {
        for (auto& checker : checkers) {
            auto error = checker(ctx, value, name, location);
            if (error.has_value()) {
                auto value_str = str_from_js(ctx, value);
                return fmt::format(
                    "Invalid property `{}` of value `{}` supplied to {}.", name,
                    value_str, location);
            }
        }
        return std::nullopt;
    };
}

Checker make_enum(std::initializer_list<JsValueWrapper> values) {
    return [values](JSContextRef ctx, JSValueRef value, const std::string& name,
                    const std::string& location) -> CheckResult {
        for (auto& expected_value : values) {
            if (JSValueIsStrictEqual(ctx, expected_value.get(), value)) {
                return std::nullopt;
            }
        }
        auto value_str = str_from_js(ctx, value);
        return fmt::format(
            "Invalid property `{}` of value `{}` supplied to {}.", name,
            value_str, location);
        // TODO expected one of {} : values
    };
}

}  // namespace aardvark::js::check_types
