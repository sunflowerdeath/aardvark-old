#pragma once

#include <functional>
#include <optional>

#include "jsi.hpp"

namespace aardvark::jsi {

struct CheckErrorParams {
    std::string kind;
    std::string name;
    std::string target;
};

using CheckResult = std::optional<std::string>;

std::string get_type_name(const Value& value);

CheckResult check_type(
    const Context& ctx,
    const Value& value,
    const std::string& expected_type,
    const CheckErrorParams& err_params);

}  // namespace aardvark::jsi
