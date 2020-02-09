#include "mappers.hpp"

namespace aardvark::jsi {

template <typename T>
using FromJsResult = tl::expected<T, std::string>;

Mapper<bool>* bool_mapper = new SimpleMapper<bool>(
    [](Context& ctx, const bool& val) { return ctx.value_make_bool(val); },
    [](Context& ctx,
       const Value& val,
       const CheckErrorParams& err_params) -> FromJsResult<bool> {
        auto err = check_type(ctx, val, "boolean", err_params);
        if (err.has_value()) return tl::make_unexpected(err.value());
        return val.to_bool().value();
    });

Mapper<float>* float_mapper = new SimpleMapper<float>(
    [](Context& ctx, const float& value) {
        return ctx.value_make_number(value);
    },
    [](Context& ctx,
       const Value& val,
       const CheckErrorParams& err_params) -> FromJsResult<float> {
        auto err = check_type(ctx, val, "number", err_params);
        if (err.has_value()) return tl::make_unexpected(err.value());
        return val.to_number().value();
    });

Mapper<double>* double_mapper = new SimpleMapper<double>(
    [](Context& ctx, const double& value) {
        return ctx.value_make_number(value);
    },
    [](Context& ctx,
       const Value& val,
       const CheckErrorParams& err_params) -> FromJsResult<double> {
        auto err = check_type(ctx, val, "number", err_params);
        if (err.has_value()) return tl::make_unexpected(err.value());
        return val.to_number().value();
    });

Mapper<int>* int_mapper = new SimpleMapper<int>(
    [](Context& ctx, const int& value) { return ctx.value_make_number(value); },
    [](Context& ctx,
       const Value& val,
       const CheckErrorParams& err_params) -> FromJsResult<int> {
        auto err = check_type(ctx, val, "number", err_params);
        if (err.has_value()) return tl::make_unexpected(err.value());
        return static_cast<int>(val.to_number().value());
    });

Mapper<std::string>* string_mapper = new SimpleMapper<std::string>(
    [](Context& ctx, const std::string& value) {
        return ctx.value_make_string(ctx.string_make_from_utf8(value));
    },
    [](Context& ctx,
       const Value& val,
       const CheckErrorParams& err_params) -> FromJsResult<std::string> {
        auto err = check_type(ctx, val, "string", err_params);
        if (err.has_value()) return tl::make_unexpected(err.value());
        return val.to_string().value().to_utf8();
    });

}  // namespace aardvark::jsi
