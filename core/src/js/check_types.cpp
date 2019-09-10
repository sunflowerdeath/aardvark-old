#include "check_types.hpp"

#include "fmt/format.h"

namespace aardvark::js::check_types {

bool to_exception(const Checker& checker, JSContextRef ctx, JSValueRef value,
                  const std::string& name, const std::string& location,
                  JSValueRef* exception) {
    auto error = checker(ctx, value, name, location);
    if (error.has_value()) {
        auto message = str_to_js(ctx, error.value());
        *exception = JSObjectMakeError(ctx, 1, &message, nullptr);
        return true;
    }
    return false;
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

Checker optional(Checker checker) {
    return
        [checker](JSContextRef ctx, JSValueRef value, const std::string& name,
                  const std::string& location) -> CheckResult {
            if (JSValueIsNull(ctx, value) || JSValueIsUndefined(ctx, value)) {
                return std::nullopt;
            }
            return checker(ctx, value, name, location);
        };
}

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
            auto keys = JSObjectCopyPropertyNames(ctx, object);
            auto keys_count = JSPropertyNameArrayGetCount(keys);
            CheckResult result = std::nullopt;
            for (auto i = 0; i < keys_count; i++) {
                auto key = JSPropertyNameArrayGetNameAtIndex(keys, i);
                auto key_value = JSObjectGetProperty(ctx, object, key, nullptr);
                auto key_result =
                    checker(ctx, key_value, name + "." + str_from_js_str(key),
                            location);
                if (key_result.has_value()) {
                    result = key_result;
                    break;
                }
            }
            JSPropertyNameArrayRelease(keys);
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

Checker make_union(std::vector<Checker> checkers) {
    return [checkers](JSContextRef ctx, JSValueRef value,
                      const std::string& name,
                      const std::string& location) -> CheckResult {
        for (auto& checker : checkers) {
            auto error = checker(ctx, value, name, location);
            if (!error.has_value()) return std::nullopt;
        }
        return fmt::format(
            "Invalid property `{}` of value `{}` supplied to {}.", name,
            str_from_js(ctx, value), location);
    };
}

Checker make_enum(std::vector<JsValueWrapper> values) {
    return [values](JSContextRef ctx, JSValueRef value, const std::string& name,
                    const std::string& location) -> CheckResult {
        for (auto& expected_value : values) {
            if (JSValueIsStrictEqual(ctx, expected_value.get(), value)) {
                return std::nullopt;
            }
        }
        return fmt::format(
            "Invalid property `{}` of value `{}` supplied to {}.", name,
            str_from_js(ctx, value), location);
    };
}

Checker make_enum_with_ctx(std::weak_ptr<JSGlobalContextWrapper> ctx,
                           std::vector<JSValueRef> values) {
    auto protected_values = std::vector<JsValueWrapper>(values.size());
    for (auto value : values) protected_values.emplace_back(ctx, value);
    return make_enum(protected_values);
}

std::string stringify_keys(JSPropertyNameArrayRef keys, int keys_count) {
    auto result = std::string();
    for (auto i = 0; i < keys_count; i++) {
        result +=
            str_from_js_str(JSPropertyNameArrayGetNameAtIndex(keys, i));
        if (i != keys_count) result += ", ";
    }
    return result;
}

Checker make_shape(std::unordered_map<std::string, Checker> shape, bool loose) {
    return [shape, loose](JSContextRef ctx, JSValueRef value,
                          const std::string& name,
                          const std::string& location) -> CheckResult {
        auto object = JSValueToObject(ctx, value, nullptr);
        for (auto& it : shape) {
            auto key = JsStringWrapper(it.first);
            auto value = JSValueRef();
            if (JSObjectHasProperty(ctx, object, key.get())) {
                value = JSObjectGetProperty(ctx, object, key.get(), nullptr);
            } else {
                value = JSValueMakeUndefined(ctx);
            }
            auto error = it.second(ctx, value, name + "." + it.first, location);
            if (error.has_value()) return error;
        }

        if (!loose) {
            auto keys = JSObjectCopyPropertyNames(ctx, object);
            auto keys_count = JSPropertyNameArrayGetCount(keys);
            for (auto i = 0; i < keys_count; i++) {
                auto key =
                    str_from_js_str(JSPropertyNameArrayGetNameAtIndex(keys, i));
                if (shape.find(key) == shape.end()) {
                    auto string_keys = stringify_keys(keys, keys_count);
                    JSPropertyNameArrayRelease(keys);
                    if (name.empty()) {
                        return fmt::format(
                            "Invalid property `{}` supplied to {}. "
                            "Valid properties are: {}.",
                            key, location, string_keys);
                    } else {
                        return fmt::format(
                            "Invalid key `{}` supplied to property `{}` of  {}."
                            "Valid keys are: {}.",
                            key, name, location, string_keys);
                    }
                }
            }
            JSPropertyNameArrayRelease(keys);
        }

        return std::nullopt;
    };
}

}  // namespace aardvark::js::check_types
