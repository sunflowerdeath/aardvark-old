#pragma once

#include <aardvark/element.hpp>
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

}  // namespace aardvark::js
