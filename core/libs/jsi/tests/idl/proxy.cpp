#include "proxy.hpp"

Result<Value> class_prop_get_proxy(
    Context& ctx, std::shared_ptr<ProxyClass>& this_val, Mapper<int>& mapper) {
    return ctx.value_make_number(2);
}

Result<bool> class_prop_set_proxy(
    Context& ctx,
    std::shared_ptr<ProxyClass>& this_val,
    Value& val,
    Mapper<int>& mapper,
    CheckErrorParams& err_params) {
    this_val->prop = 2;
    return true;
}

Result<Value> class_method_proxy(
    Context& ctx,
    std::shared_ptr<ProxyClass>& this_val,
    int& val,
    Mapper<int>& mapper) {
    return ctx.value_make_number(this_val->proxied_method(val));
}
