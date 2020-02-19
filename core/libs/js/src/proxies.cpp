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

}  // namespace aardvark::js
