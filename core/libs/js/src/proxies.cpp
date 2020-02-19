#include "proxies.hpp"

namespace aardvark::js {

jsi::Value element_get_parent_proxy(
    jsi::Context& ctx,
    std::shared_ptr<Element>& elem,
    jsi::ObjectsMapper2<Element, Element>& elem_mapper) {
    // TODO
    return ctx.value_make_undefined();
}

jsi::Value element_get_document_proxy(
    jsi::Context& ctx,
    std::shared_ptr<Element>& elem,
    jsi::ObjectsMapper2<Document, Document>& elem_mapper) {
    // TODO
    return ctx.value_make_undefined();
}

}  // namespace aardvark::js
