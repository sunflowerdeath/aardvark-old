#include "api/transform.hpp"

namespace aardvark::js {

jsi::Value transform_to_js(jsi::Context& ctx, const Transform& transform) {
    auto arr = ctx.object_make_array();
    for (auto i = 0; i < 9; i++) {
        arr.set_property_at_index(
            i, ctx.value_make_number(transform.values[i]));
    }
    return arr.to_value();
}

tl::expected<Transform, std::string> transform_try_from_js(
    jsi::Context& ctx,
    const jsi::Value& value,
    const jsi::CheckErrorParams& err_params) {
    auto res = jsi::check_type(ctx, value, "array", err_params);
    if (res.has_value()) return tl::make_unexpected(res.value());
    auto arr = value.to_object().value();
    auto transform = Transform();
    for (auto i = 0; i < 9; i++) {
        auto i_val = arr.get_property_at_index(i).value(); // TODO
        auto res = jsi::float_mapper->try_from_js(ctx, i_val, err_params);
        if (res.has_value())
            transform.values[i] = res.value();
        else
            return tl::make_unexpected(res.error());
    }
    return transform;
}

}  // namespace aardvark::js
