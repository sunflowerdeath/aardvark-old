#pragma once

#include "JavaScriptCore/JavaScript.h"
#include "bindings_host.hpp"

namespace aardvark::js {

JSClassRef element_create_class();
JSObjectRef element_call_as_constructor(JSContextRef ctx,
                                        JSObjectRef constructor,
                                        size_t argument_count,
                                        const JSValueRef arguments[],
                                        JSValueRef* exception);

template <typename T>
JSObjectRef elem_constructor(JSContextRef ctx, JSObjectRef constructor,
                             size_t argument_count,
                             const JSValueRef arguments[],
                             JSValueRef* exception) {
    return BindingsHost::get(ctx)->element_index->create_js_object(
        std::make_shared<T>());
}

JSClassRef align_elem_create_class(JSClassRef parent_class);
JSClassRef background_elem_create_class(JSClassRef parent_class);
JSClassRef center_elem_create_class(JSClassRef parent_class);
JSClassRef intrinsic_height_elem_create_class(JSClassRef parent_class);
JSClassRef intrinsic_width_elem_create_class(JSClassRef parent_class);
JSClassRef flex_elem_create_class(JSClassRef parent_class);
JSClassRef flex_child_elem_create_class(JSClassRef parent_class);
JSClassRef padding_elem_create_class(JSClassRef parent_class);
JSClassRef responder_elem_create_class(JSClassRef parent_class);
JSClassRef scroll_elem_create_class(JSClassRef parent_class);
JSClassRef sized_elem_create_class(JSClassRef parent_class);
JSClassRef stack_elem_create_class(JSClassRef parent_class);
JSClassRef text_elem_create_class(JSClassRef parent_class);
JSClassRef translate_elem_create_class(JSClassRef parent_class);

}  // namespace aardvark::js
