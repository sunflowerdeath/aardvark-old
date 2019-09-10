#include "typedefs.hpp"

#include "fmt/format.h"
#include "helpers.hpp"

namespace aardvark::js {

Typedefs::Typedefs(BindingsHost* host) : host(host) {
    value = check_types::make_shape(
        {{"type", check_types::make_enum_with_ctx(
                      host->ctx, {str_to_js(host->ctx->get(), "abs"),
                                  str_to_js(host->ctx->get(), "rel"),
                                  str_to_js(host->ctx->get(), "none")})},
         {"value", check_types::number}});

    alignment = check_types::make_shape(
        {{"left", value}, {"right", value}, {"top", value}, {"bottom", value}});

    color_component =
        [](JSContextRef ctx, JSValueRef value, const std::string& kind,
           const std::string& name,
           const std::string& target) -> check_types::CheckResult {
        auto error = check_types::number(ctx, value, kind, name, target);
        if (error.has_value()) return error;
        auto number = int_from_js(ctx, value);
        if (number >= 0 && number <= 255) return std::nullopt;
        return fmt::format(
            "Invalid {} `{}` of value {} supplied to {}, "
            "expected number between 0 and 255.",
            kind, name, number, target);
    };

    color = check_types::make_shape({{"alpha", color_component},
                                     {"red", color_component},
                                     {"green", color_component},
                                     {"blue", color_component}});

    element = check_types::instance_of(host->element_class);
}

}
