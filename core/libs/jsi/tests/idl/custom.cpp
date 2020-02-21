#include "custom.hpp"

Value custom_to_js(Context& ctx, const CustomType& val) {
    return ctx.value_make_number(val[0]);
}

tl::expected<CustomType, std::string> custom_try_from_js(
    Context& ctx, const Value& val, const CheckErrorParams& err_params) {
    auto res = int_mapper->try_from_js(ctx, val, err_params);
    if (!res.has_value()) return tl::make_unexpected(res.error());
    return CustomType{res.value(), res.value()};
}
