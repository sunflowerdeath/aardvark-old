#pragma once

#include <aardvark/base_types.hpp>
#include <aardvark_jsi/jsi.hpp>
#include <aardvark_jsi/mappers.hpp>

namespace aardvark::js {

jsi::Value transform_to_js(jsi::Context& ctx, const Transform& transform);

tl::expected<Transform, std::string> transform_try_from_js(
    jsi::Context& ctx,
    const jsi::Value& value,
    const jsi::CheckErrorParams& err_params);

}  // namespace aardvark::js
