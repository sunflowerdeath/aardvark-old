#pragma once

#include <aardvark/element.hpp>
#include <aardvark/elements/flex.hpp>
#include <aardvark_jsi/jsi.hpp>
#include <aardvark_jsi/mappers.hpp>

namespace aardvark::js {

jsi::Value element_get_parent_proxy(
    jsi::Context& ctx,
    std::shared_ptr<Element>& elem,
    jsi::Mapper<std::shared_ptr<Element>>& mapper);

jsi::Value element_get_document_proxy(
    jsi::Context& ctx,
    std::shared_ptr<Element>& elem,
    jsi::Mapper<std::shared_ptr<Document>>& mapper);

jsi::Value flex_child_get_align_proxy(
    jsi::Context& ctx,
    std::shared_ptr<FlexChildElement> elem,
    jsi::Mapper<FlexAlign>& mapper);

jsi::Result<bool> flex_child_set_align_proxy(
    jsi::Context& ctx,
    std::shared_ptr<FlexChildElement> elem,
    jsi::Value& val,
    jsi::Mapper<FlexAlign>& mapper,
    const jsi::CheckErrorParams& err_params);

}  // namespace aardvark::js
