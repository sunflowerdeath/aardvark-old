#include "mappers.hpp"

namespace aardvark::jsi {

Mapper<bool>* bool_mapper = new SimpleMapper<bool>(
    [](Context& ctx, const bool& value) { return ctx.value_make_bool(value); },
    [](Context& ctx, const Value& value) { return value.to_bool(); },
    &boolean_checker  // checker
);

Mapper<double>* number_mapper = new SimpleMapper<double>(
    [](Context& ctx, const double& value) {
        return ctx.value_make_number(value);
    },  // to_js
    [](Context& ctx, const Value& value) {
        return value.to_number();
    },               // from_js
    &number_checker  // checker
);

Mapper<int>* int_mapper = new SimpleMapper<int>(
    [](Context& ctx, const int& value) {
        return ctx.value_make_number(value);
    },  // to_js
    [](Context& ctx, const Value& value) {
        return static_cast<int>(value.to_number());
    },               // from_js
    &number_checker  // checker
);

Mapper<std::string>* string_mapper = new SimpleMapper<std::string>(
    [](Context& ctx, const std::string& value) {
        return ctx.value_make_string(ctx.string_make_from_utf8(value));
    },  // to_js
    [](Context& ctx, const Value& value) {
        return value.to_string().to_utf8();
    },               // from_js
    &string_checker  // checker
);

}  // namespace aardvark::jsi
