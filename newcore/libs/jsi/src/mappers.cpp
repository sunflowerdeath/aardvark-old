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

}  // namespace aardvark::jsi
