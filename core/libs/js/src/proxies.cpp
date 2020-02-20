#include "proxies.hpp"

namespace aardvark::js {

jsi::Value element_get_parent_proxy(
    jsi::Context& ctx,
    std::shared_ptr<Element>& elem,
    jsi::Mapper<std::shared_ptr<Element>>& mapper) {
    if (elem->document == nullptr) return ctx.value_make_undefined();
    return mapper.to_js(ctx, elem->parent->shared_from_this());
}

jsi::Value element_get_document_proxy(
    jsi::Context& ctx,
    std::shared_ptr<Element>& elem,
    jsi::Mapper<std::shared_ptr<Document>>& mapper) {
    if (elem->document == nullptr) return ctx.value_make_undefined();
    return mapper.to_js(ctx, elem->document->shared_from_this());
}

jsi::Value flex_child_get_align_proxy(
    jsi::Context& ctx,
    std::shared_ptr<FlexChildElement> elem,
    jsi::Mapper<FlexAlign>& mapper) {
    if (elem->align == std::nullopt) return ctx.value_make_undefined();
    return mapper.to_js(ctx, elem->align.value());
}

jsi::Result<bool> flex_child_set_align_proxy(
    jsi::Context& ctx,
    std::shared_ptr<FlexChildElement> elem,
    jsi::Value& val,
    jsi::Mapper<FlexAlign>& mapper,
    const jsi::CheckErrorParams& err_params) {
    if (val.get_type() == jsi::ValueType::undefined) elem->align = std::nullopt;
    auto res = mapper.try_from_js(ctx, val, err_params);
    if (res.has_value()) {
        elem->align = res.value();
        return true;
    } else {
        return make_error_result(ctx, res.error());
    }
}

}  // namespace aardvark::js
