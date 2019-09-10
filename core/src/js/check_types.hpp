#pragma once

#include <optional>
#include <functional>
#include "JavaScriptCore/JavaScript.h"
#include "helpers.hpp"

namespace aardvark::js::check_types {

using CheckResult = std::optional<std::string>;

using Checker = std::function<CheckResult(
    JSContextRef, JSValueRef, const std::string&, const std::string&)>;

bool to_exception(const Checker& checker, JSContextRef ctx, JSValueRef value,
                  const std::string& name, const std::string& location,
                  JSValueRef* exception);

extern Checker number;
extern Checker boolean;
extern Checker object;
extern Checker array;
extern Checker string;
extern Checker symbol;

Checker optional(Checker checker);
Checker array_of(Checker checker);
Checker object_of(Checker checker);
Checker instance_of(JSClassRef cls);
Checker make_union(std::vector<Checker> checkers);
Checker make_enum(std::vector<JsValueWrapper> values);
Checker make_enum_with_ctx(std::weak_ptr<JSGlobalContextWrapper> ctx,
                           std::vector<JSValueRef> values);
Checker make_shape(std::unordered_map<std::string, Checker> shape,
                   bool loose = false);

}  // namespace aardvark::js::check_types
