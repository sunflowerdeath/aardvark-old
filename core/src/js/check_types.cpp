#include "check_types.hpp"

#include "fmt/format.h"

namespace aardvark::js::check_types {

bool to_exception(const CheckResult& result, JSContextRef ctx,
                  JSValueRef* exception) {
    if (!result.has_value()) return false;
    auto message = str_to_js(ctx, result.value());
    *exception = JSObjectMakeError(ctx, 1, &message, nullptr);
    return true;
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
                           const ErrorParams& params) -> CheckResult {
        auto type = get_type(ctx, value);
        if (type == expected_type) return std::nullopt;
        return fmt::format(
            "Invalid {} `{}` of type `{}` supplied to `{}`, expected `{}`.",
            params.kind, params.name, type, params.target, expected_type);
    };
}

Checker number = make_primitive("number");
Checker boolean = make_primitive("boolean");
Checker object = make_primitive("object");
Checker array = make_primitive("array");
Checker string = make_primitive("string");
Checker symbol = make_primitive("symbol");

Checker optional(Checker checker) {
    return [checker](JSContextRef ctx, JSValueRef value,
                     const ErrorParams& params) -> CheckResult {
        if (JSValueIsNull(ctx, value) || JSValueIsUndefined(ctx, value)) {
            return std::nullopt;
        }
        return checker(ctx, value, params);
    };
}

Checker array_of(Checker checker) {
    return [checker](JSContextRef ctx, JSValueRef value,
                     const ErrorParams& params) -> CheckResult {
        auto array_error = array(ctx, value, params);
        if (array_error.has_value()) return array_error;

        auto object = JSValueToObject(ctx, value, nullptr);
        auto length = 0;
        map_prop_from_js<int, int_from_js>(ctx, object, "length", &length);
        JSValueRef exception;
        for (auto i = 0; i < length; i++) {
            auto item = JSObjectGetPropertyAtIndex(ctx, object, i, &exception);
            auto item_params = ErrorParams{
                params.kind, params.name + "[" + std::to_string(i) + "]",
                params.target};
            auto error = checker(ctx, item, item_params);
            if (error.has_value()) return error;
        }

        return std::nullopt;
    };
}

Checker object_of(Checker checker) {
    return [checker](JSContextRef ctx, JSValueRef value,
                     const ErrorParams& params) -> CheckResult {
        auto object_error = object(ctx, value, params);
        if (object_error.has_value()) return object_error;

        auto object = JSValueToObject(ctx, value, nullptr);
        auto keys = JSObjectCopyPropertyNames(ctx, object);
        auto keys_count = JSPropertyNameArrayGetCount(keys);
        CheckResult result = std::nullopt;
        for (auto i = 0; i < keys_count; i++) {
            auto key = JSPropertyNameArrayGetNameAtIndex(keys, i);
            auto key_value = JSObjectGetProperty(ctx, object, key, nullptr);
            auto key_params = ErrorParams{
                params.kind, params.name + "." + str_from_js_str(key),
                params.target};
            auto key_result = checker(ctx, key_value, key_params);
            if (key_result.has_value()) {
                result = key_result;
                break;
            }
        }
        JSPropertyNameArrayRelease(keys);
        return result;
    };
}

Checker instance_of(JSObjectRef constructor) {
    return [constructor](JSContextRef ctx, JSValueRef value,
                         const ErrorParams& params) -> CheckResult {
        auto object_error = object(ctx, value, params);
        if (object_error.has_value()) return object_error;
        if (JSValueIsInstanceOfConstructor(ctx, value, constructor, nullptr)) {
            return std::nullopt;
        }
        // TODO cls names
        auto actual_class_name = "ACTUAL_CLASS_NAME";
        auto expected_class_name = "EXPECTED_CLASS_NAME";
        return fmt::format(
            "Invalid {} `{}` of type `{}` supplied to `{}`, expected "
            "instance of `{}`.",
            params.kind, params.name, actual_class_name, params.target,
            expected_class_name);
    };
}

Checker make_union(std::vector<Checker> checkers) {
    return [checkers](JSContextRef ctx, JSValueRef value,
                      const ErrorParams& params) -> CheckResult {
        for (auto& checker : checkers) {
            auto error = checker(ctx, value, params);
            if (!error.has_value()) return std::nullopt;
        }
        return fmt::format("Invalid {} `{}` of value `{}` supplied to `{}`.",
                           params.kind, params.name, str_from_js(ctx, value),
                           params.target);
    };
}

Checker make_enum(std::vector<JsValueWrapper> values) {
    return [values](JSContextRef ctx, JSValueRef value,
                    const ErrorParams& params) -> CheckResult {
        for (auto& expected_value : values) {
            if (JSValueIsStrictEqual(ctx, expected_value.get(), value)) {
                return std::nullopt;
            }
        }
        // TODO stringify values
        return fmt::format("Invalid {} `{}` of value `{}` supplied to `{}`.",
                           params.kind, params.name, str_from_js(ctx, value),
                           params.target);
    };
}

Checker make_enum_with_ctx(std::weak_ptr<JsGlobalContextWrapper> ctx,
                           std::vector<JSValueRef> values) {
    auto protected_values = std::vector<JsValueWrapper>(values.size());
    for (auto value : values) protected_values.emplace_back(ctx, value);
    return make_enum(protected_values);
}

std::string stringify_keys(JSPropertyNameArrayRef keys, int keys_count) {
    auto result = std::string();
    for (auto i = 0; i < keys_count; i++) {
        result += str_from_js_str(JSPropertyNameArrayGetNameAtIndex(keys, i));
        if (i != keys_count) result += ", ";
    }
    return result;
}

Checker make_shape(std::unordered_map<std::string, Checker> shape, bool loose) {
    return [shape, loose](JSContextRef ctx, JSValueRef value,
                          const ErrorParams& params) -> CheckResult {
        auto object = JSValueToObject(ctx, value, nullptr);
        for (auto& it : shape) {
            auto key = JsStringWrapper(it.first);
            JSValueRef key_value;
            if (JSObjectHasProperty(ctx, object, key.get())) {
                key_value =
                    JSObjectGetProperty(ctx, object, key.get(), nullptr);
            } else {
                key_value = JSValueMakeUndefined(ctx);
            }
            auto key_params = ErrorParams{
                params.kind, params.name + "." + it.first, params.target};
            auto error = it.second(ctx, key_value, key_params);
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
                    if (params.name.empty()) {
                        return fmt::format(
                            "Invalid key `{}` supplied to {}. "
                            "Valid keys are: {}.",
                            key, params.target, string_keys);
                    } else {
                        return fmt::format(
                            "Invalid key `{}` supplied to {} `{}` of `{}`."
                            "Valid keys are: {}.",
                            key, params.kind, params.name, params.target,
                            string_keys);
                    }
                }
            }
            JSPropertyNameArrayRelease(keys);
        }

        return std::nullopt;
    };
}

ArgumentsChecker make_arguments(
    std::vector<std::pair<std::string, Checker>> args_checkers) {
    return [args_checkers](JSContextRef ctx, int args_count,
                           const JSValueRef args[],
                           const std::string& target) -> CheckResult {
        if (args_checkers.size() != args_count) {
            return fmt::format(
                "Invalid number of arguments supplied to {}. "
                "Expected {} arguments, got {}.",
                target, args_checkers.size(), args_count);
        }
        for (auto i = 0; i < args_count; i++) {
            auto& [name, checker] = args_checkers[i];
            auto error = checker(ctx, args[i], {"argument", name, target});
            if (error.has_value()) return error;
        }
        return std::nullopt;
    };
}

}  // namespace aardvark::js::check_types
