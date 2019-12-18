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

using Checker = std::function<CheckResult(
    const Context& ctx, const Value& value, const CheckErrorParams&)>;

extern Checker boolean_checker;
extern Checker number_checker;
extern Checker string_checker;
extern Checker symbol_checker;
extern Checker object_checker;
extern Checker array_checker;
extern Checker function_checker;

}  // namespace aardvark::jsi
