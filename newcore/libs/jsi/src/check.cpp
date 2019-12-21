#include "check.hpp"

#include "fmt/format.h"

namespace aardvark::jsi {

std::string get_type_name(const Value& value) {
    auto type = value.get_type();
    switch (type) {
        case ValueType::null:
            return "null";
            break;
        case ValueType::undefined:
            return "undefined";
            break;
        case ValueType::number:
            return "number";
            break;
        case ValueType::boolean:
            return "boolean";
            break;
        case ValueType::string:
            return "string";
            break;
        case ValueType::symbol:
            return "symbol";
            break;
        case ValueType::object:
            auto object = value.to_object().value();
            if (object.is_array()) return "array";
            if (object.is_function()) return "function";
            return "object";
            break;
    }
}

Checker make_primitive(const std::string& expected_type) {
    return [expected_type](
               const Context& ctx, const Value& value,
               const CheckErrorParams& params) -> CheckResult {
        auto type = get_type_name(value);
        if (type == expected_type) return std::nullopt;
        if (expected_type == "object" &&
            (type == "function" || type == "array")) {
            return std::nullopt;
        }
        return fmt::format(
            "Invalid {} `{}` of type `{}` supplied to `{}`, expected `{}`.",
            params.kind, params.name, type, params.target, expected_type);
    };
}

Checker number_checker = make_primitive("number");
Checker boolean_checker = make_primitive("boolean");
Checker string_checker = make_primitive("string");
Checker symbol_checker = make_primitive("symbol");
Checker object_checker = make_primitive("object");
Checker array_checker = make_primitive("array");
Checker function_checker = make_primitive("function");

}  // namespace aardvark::jsi
