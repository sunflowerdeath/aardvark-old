#pragma once

#include <aardvark_jsi/jsi.hpp>
#include <aardvark_jsi/mappers.hpp>
#include <array>

using CustomType = std::array<int, 2>;

using namespace aardvark::jsi;

Value custom_to_js(Context& ctx, const CustomType& val);

tl::expected<CustomType, std::string> custom_try_from_js(
    Context& ctx, const Value& val, const CheckErrorParams& err_params);
