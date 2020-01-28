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

std::string format_name(const std::string& name) {
    return name.empty() ? "" : fmt::format(" `{}`", name);
}

CheckResult check_type(
    const Context& ctx,
    const Value& value,
    const std::string& expected_type,
    const CheckErrorParams& err_params) {
    auto type = get_type_name(value);
    if (type == expected_type) return std::nullopt;
    if (expected_type == "object" && (type == "function" || type == "array")) {
        return std::nullopt;
    }
    return fmt::format(
        "Invalid {}{} of type `{}` supplied to `{}`, expected `{}`.",
        err_params.kind,
        format_name(err_params.name),
        type,
        err_params.target,
        expected_type);
}

}  // namespace aardvark::jsi
