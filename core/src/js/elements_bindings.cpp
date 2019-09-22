#include "elements_bindings.hpp"

#include "../elements/elements.hpp"
#include "helpers.hpp"
#include "base_types_bindings.hpp"
#include "events_bindings.hpp"
#include "function_wrapper.hpp"

namespace aardvark::js {

std::shared_ptr<Element> get_elem(JSContextRef ctx, JSObjectRef object) {
    return BindingsHost::get(ctx)->element_index->get_native_object(object);
}

template <class E>
E* get_elem(JSContextRef ctx, JSObjectRef object) {
    return dynamic_cast<E*>(get_elem(ctx, object).get());
}

JSClassDefinition create_elem_class_definition(const char* name,
                                               JSClassRef parent_class) {
    auto definition = kJSClassDefinitionEmpty;
    definition.className = name;
    definition.parentClass = parent_class;
    return definition;
}

//------------------------------------------------------------------------------
// Element
//------------------------------------------------------------------------------

JSValueRef element_get_width(JSContextRef ctx, JSObjectRef object,
                             JSStringRef property_name, JSValueRef* exception) {
    auto elem = get_elem(ctx, object);
    return JSValueMakeNumber(ctx, static_cast<double>(elem->size.width));
}

JSValueRef element_get_height(JSContextRef ctx, JSObjectRef object,
                              JSStringRef property_name,
                              JSValueRef* exception) {
    auto elem = get_elem(ctx, object);
    return JSValueMakeNumber(ctx, static_cast<double>(elem->size.height));
}

JSValueRef element_get_left(JSContextRef ctx, JSObjectRef object,
                            JSStringRef property_name, JSValueRef* exception) {
    auto elem = get_elem(ctx, object);
    return JSValueMakeNumber(ctx, static_cast<double>(elem->abs_position.left));
}

JSValueRef element_get_top(JSContextRef ctx, JSObjectRef object,
                           JSStringRef property_name, JSValueRef* exception) {
    auto elem = get_elem(ctx, object);
    return JSValueMakeNumber(ctx, static_cast<double>(elem->abs_position.top));
}

JSValueRef element_get_parent(JSContextRef ctx, JSObjectRef object,
                              JSStringRef property_name,
                              JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto elem = host->element_index->get_native_object(object);
    auto parent = elem->parent;
    if (parent == nullptr) {
        return JSValueMakeUndefined(ctx);
    } else {
        return host->element_index->get_js_object(parent);
    }
}

JSValueRef element_get_children(JSContextRef ctx, JSObjectRef object,
                              JSStringRef property_name,
                              JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto elem = get_elem(ctx, object);
    auto items = std::vector<JSValueRef>();
    elem->visit_children([&host, &items](std::shared_ptr<Element> child) {
        items.push_back(host->element_index->get_js_object(child.get()));
    });
    return JSObjectMakeArray(ctx, items.size(), items.data(), nullptr);
}

JSValueRef element_append_child(JSContextRef ctx, JSObjectRef function,
                                JSObjectRef object, size_t argument_count,
                                const JSValueRef arguments[],
                                JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);

    static auto target = std::string("Element.appendChild()");
    auto checker =
        check_types::make_arguments({{"child", host->typedefs->element}});
    auto result = checker(ctx, argument_count, arguments, target);
    if (check_types::to_exception(result, ctx, exception)) {
        return JSValueMakeUndefined(ctx);
    }

    auto elem = host->element_index->get_native_object(object);
    auto child = host->element_index->get_native_object(
        JSValueToObject(ctx, arguments[0], exception));
    elem->append_child(child);
    return JSValueMakeUndefined(ctx);
}

JSValueRef element_remove_child(JSContextRef ctx, JSObjectRef function,
                                JSObjectRef object, size_t argument_count,
                                const JSValueRef arguments[],
                                JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);

    // TODO check types
  
    auto elem = host->element_index->get_native_object(object);
    auto child = host->element_index->get_native_object(
        JSValueToObject(ctx, arguments[0], exception));
    elem->remove_child(child);
    return JSValueMakeUndefined(ctx);
}

JSValueRef element_insert_before_child(JSContextRef ctx, JSObjectRef function,
                                       JSObjectRef object,
                                       size_t argument_count,
                                       const JSValueRef arguments[],
                                       JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);

    /*
    auto checker =
        check_types::make_arguments({{"child", host->typedefs->element},
                                     {"beforeChild", host->typedefs->element}});
    if (check_types::to_exception(checker, ctx, argument_count, arguments,
                                  "Element.insertBeforeChild()", exception)) {
        return JSValueMakeUndefined(ctx);
    }
    */

    auto elem = host->element_index->get_native_object(object);
    auto child = get_elem(ctx, JSValueToObject(ctx, arguments[0], exception));
    auto before_child =
        get_elem(ctx, JSValueToObject(ctx, arguments[1], exception));
    elem->insert_before_child(child, before_child);
    return JSValueMakeUndefined(ctx);
}

void element_finalize(JSObjectRef object) {
    ObjectsIndex<Element>::remove(object);
}

JSClassRef element_create_class() {
    auto definition = kJSClassDefinitionEmpty;
    JSStaticFunction static_functions[] = {
        {"appendChild", element_append_child, PROP_ATTR_STATIC},
        {"removeChild", element_remove_child, PROP_ATTR_STATIC},
        {"insertBeforeChild", element_insert_before_child, PROP_ATTR_STATIC},
        {0, 0, 0}};
    JSStaticValue static_values[] = {
        {"width", element_get_width, nullptr, PROP_ATTR_STATIC},
        {"height", element_get_height, nullptr, PROP_ATTR_STATIC},
        {"left", element_get_left, nullptr, PROP_ATTR_STATIC},
        {"top", element_get_top, nullptr, PROP_ATTR_STATIC},
        {"parent", element_get_parent, nullptr, PROP_ATTR_STATIC},
        {"children", element_get_children, nullptr, PROP_ATTR_STATIC},
        {0, 0, 0, 0}};
    definition.className = "Element";
    definition.finalize = element_finalize;
    definition.staticFunctions = static_functions;
    definition.staticValues = static_values;
    return JSClassCreate(&definition);
}

JSObjectRef element_call_as_constructor(JSContextRef ctx,
                                        JSObjectRef constructor,
                                        size_t argument_count,
                                        const JSValueRef arguments[],
                                        JSValueRef* exception) {
    auto message =
        str_to_js(ctx, "Element's constructor can't be called directly.");
    *exception = JSObjectMakeError(ctx, 1, &message, nullptr);
    return nullptr;
}

//------------------------------------------------------------------------------
// Align
//------------------------------------------------------------------------------

bool align_element_set_align(JSContextRef ctx, JSObjectRef object,
                             JSStringRef property_name, JSValueRef value,
                             JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);

    // TODO check types
    /*
    auto params = check_types::params{"property", "alignment", "AlignElement"};
    if (check_types::to_exception(host->typedefs->alignment, ctx, value, params,
                                  exception)) {
        return false;
    }
    */

    auto elem = get_elem<elements::Align>(ctx, object);
    auto insets = alignment_from_js(ctx, JSValueToObject(ctx, value, nullptr));
    elem->insets = insets;
    elem->change();
    return true;
}

JSValueRef align_element_get_align(JSContextRef ctx, JSObjectRef object,
                                   JSStringRef property_name,
                                   JSValueRef* exception) {
    auto elem = get_elem<elements::Align>(ctx, object);
    return alignment_to_js(ctx, elem->insets);
}

JSClassRef align_elem_create_class(JSClassRef parent_class) {
    auto definition =
        create_elem_class_definition("AlignElement", parent_class);
    JSStaticValue static_values[] = {
        {"align", align_element_get_align, align_element_set_align,
         kJSPropertyAttributeNone},
        {0, 0, 0, 0}};
    definition.staticValues = static_values;
    return JSClassCreate(&definition);
}

//------------------------------------------------------------------------------
// Background
//------------------------------------------------------------------------------

JSValueRef background_element_get_color(JSContextRef ctx, JSObjectRef object,
                                        JSStringRef property_name,
                                        JSValueRef* exception) {
    auto elem = get_elem<elements::Background>(ctx, object);
    return color_to_js(ctx, elem->color);
}

bool background_element_set_color(JSContextRef ctx, JSObjectRef object,
                                  JSStringRef property_name, JSValueRef value,
                                  JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);

    static auto params =
        check_types::ErrorParams{"property", "color", "BackgroundElement"};
    auto error = host->typedefs->color(ctx, value, params);
    if (check_types::to_exception(error, ctx, exception)) return false;

    auto elem = get_elem<elements::Background>(ctx, object);
    elem->color = color_from_js(ctx, value);
    elem->change();
    return true;
}

JSClassRef background_elem_create_class(JSClassRef parent_class) {
    auto definition =
        create_elem_class_definition("BackgroundElement", parent_class);
    JSStaticValue static_values[] = {
        {"color", background_element_get_color, background_element_set_color,
         kJSPropertyAttributeNone},
        {0, 0, 0, 0}};
    definition.staticValues = static_values;
    return JSClassCreate(&definition);
}

//------------------------------------------------------------------------------
// Center
//------------------------------------------------------------------------------

JSClassRef center_elem_create_class(JSClassRef parent_class) {
    auto definition =
        create_elem_class_definition("CenterElement", parent_class);
    return JSClassCreate(&definition);
}

//------------------------------------------------------------------------------
// IntrinsicHeight
//------------------------------------------------------------------------------

JSClassRef intrinsic_height_elem_create_class(JSClassRef parent_class) {
    auto definition =
        create_elem_class_definition("IntrinsicHeightElement", parent_class);
    return JSClassCreate(&definition);
}

//------------------------------------------------------------------------------
// Flex
//------------------------------------------------------------------------------

JSValueRef flex_elem_get_direction(JSContextRef ctx, JSObjectRef object,
                             JSStringRef property_name, JSValueRef* exception) {
    auto elem = get_elem<elements::Flex>(ctx, object);
    return int_to_js(ctx, static_cast<int>(elem->direction));
}

bool flex_elem_set_direction(JSContextRef ctx, JSObjectRef object,
                             JSStringRef property_name, JSValueRef value,
                             JSValueRef* exception) {
    auto elem = get_elem<elements::Flex>(ctx, object);
    elem->direction =
        static_cast<elements::FlexDirection>(int_from_js(ctx, value));
    elem->change();
    return true;
}

JSValueRef flex_elem_get_align(JSContextRef ctx, JSObjectRef object,
                               JSStringRef property_name,
                               JSValueRef* exception) {
    auto elem = get_elem<elements::Flex>(ctx, object);
    return int_to_js(ctx, static_cast<int>(elem->align));
}

bool flex_elem_set_align(JSContextRef ctx, JSObjectRef object,
                         JSStringRef property_name, JSValueRef value,
                         JSValueRef* exception) {
    auto elem = get_elem<elements::Flex>(ctx, object);
    elem->align = static_cast<elements::FlexAlign>(int_from_js(ctx, value));
    elem->change();
    return true;
}

JSValueRef flex_elem_get_justify(JSContextRef ctx, JSObjectRef object,
                                 JSStringRef property_name,
                                 JSValueRef* exception) {
    auto elem = get_elem<elements::Flex>(ctx, object);
    return int_to_js(ctx, static_cast<int>(elem->justify));
}

bool flex_elem_set_justify(JSContextRef ctx, JSObjectRef object,
                           JSStringRef property_name, JSValueRef value,
                           JSValueRef* exception) {
    auto elem = get_elem<elements::Flex>(ctx, object);
    elem->justify = static_cast<elements::FlexJustify>(int_from_js(ctx, value));
    elem->change();
    return true;
}

JSClassRef flex_elem_create_class(JSClassRef parent_class) {
    auto definition = create_elem_class_definition("FlexElement", parent_class);
    JSStaticValue static_values[] = {
        {"direction", flex_elem_get_direction, flex_elem_set_direction,
         kJSPropertyAttributeNone},
        {"align", flex_elem_get_align, flex_elem_set_align,
         kJSPropertyAttributeNone},
        {"justify", flex_elem_get_justify, flex_elem_set_justify,
         kJSPropertyAttributeNone},
        {0, 0, 0, 0}};
    definition.staticValues = static_values;
    return JSClassCreate(&definition);
}

//------------------------------------------------------------------------------
// FlexChild
//------------------------------------------------------------------------------

JSValueRef flex_child_elem_get_flex(JSContextRef ctx, JSObjectRef object,
                                 JSStringRef property_name,
                                 JSValueRef* exception) {
    auto elem = get_elem<elements::FlexChild>(ctx, object);
    return int_to_js(ctx, elem->flex);
}

bool flex_child_elem_set_flex(JSContextRef ctx, JSObjectRef object,
                           JSStringRef property_name, JSValueRef value,
                           JSValueRef* exception) {
    auto elem = get_elem<elements::FlexChild>(ctx, object);
    elem->flex = int_from_js(ctx, value);
    elem->change();
    return true;
}

JSValueRef flex_child_elem_get_align(JSContextRef ctx, JSObjectRef object,
                                 JSStringRef property_name,
                                 JSValueRef* exception) {
    auto elem = get_elem<elements::FlexChild>(ctx, object);
    if (elem->align == std::nullopt) return JSValueMakeUndefined(ctx);
    return int_to_js(ctx, static_cast<int>(elem->align.value()));
}

bool flex_child_elem_set_align(JSContextRef ctx, JSObjectRef object,
                           JSStringRef property_name, JSValueRef value,
                           JSValueRef* exception) {
    auto elem = get_elem<elements::FlexChild>(ctx, object);
    if (JSValueIsUndefined(ctx, value) || JSValueIsNull(ctx, value)) {
        elem->align = std::nullopt;
    } else {
        elem->align = static_cast<elements::FlexAlign>(int_from_js(ctx, value));
    }
    elem->change();
    return true;
}

JSValueRef flex_child_elem_get_tight_fit(JSContextRef ctx, JSObjectRef object,
                                 JSStringRef property_name,
                                 JSValueRef* exception) {
    auto elem = get_elem<elements::FlexChild>(ctx, object);
    return JSValueMakeBoolean(ctx, elem->tight_fit);
}

bool flex_child_elem_set_tight_fit(JSContextRef ctx, JSObjectRef object,
                           JSStringRef property_name, JSValueRef value,
                           JSValueRef* exception) {
    auto elem = get_elem<elements::FlexChild>(ctx, object);
    elem->tight_fit = JSValueToBoolean(ctx, value);
    elem->change();
    return true;
}

JSClassRef flex_child_elem_create_class(JSClassRef parent_class) {
    auto definition =
        create_elem_class_definition("FlexChildElement", parent_class);
    JSStaticValue static_values[] = {
        {"flex", flex_child_elem_get_flex, flex_child_elem_set_flex,
         kJSPropertyAttributeNone},
        {"align", flex_child_elem_get_align, flex_child_elem_set_align,
         kJSPropertyAttributeNone},
        {"tightFit", flex_child_elem_get_tight_fit,
         flex_child_elem_set_tight_fit, kJSPropertyAttributeNone},
        {0, 0, 0, 0}};
    definition.staticValues = static_values;
    return JSClassCreate(&definition);
}

//------------------------------------------------------------------------------
// Padding
//------------------------------------------------------------------------------

JSValueRef padding_elem_get_padding(JSContextRef ctx, JSObjectRef object,
                                      JSStringRef property_name,
                                      JSValueRef* exception) {
    auto elem = get_elem<PaddingElement>(ctx, object);
    return padding_to_js(ctx, elem->padding);
}

bool padding_elem_set_padding(JSContextRef ctx, JSObjectRef object,
                                JSStringRef property_name, JSValueRef value,
                                JSValueRef* exception) {
    auto elem = get_elem<PaddingElement>(ctx, object);
    elem->padding = padding_from_js(ctx, value);
    elem->change();
    return true;
}

JSClassRef padding_elem_create_class(JSClassRef parent_class) {
    auto definition =
        create_elem_class_definition("PaddingElement", parent_class);
    JSStaticValue static_values[] = {
        {"padding", padding_elem_get_padding, padding_elem_set_padding,
         kJSPropertyAttributeNone},
        {0, 0, 0, 0}};
    definition.staticValues = static_values;
    return JSClassCreate(&definition);
}

//------------------------------------------------------------------------------
// Responder
//------------------------------------------------------------------------------

std::vector<JSValueRef> responder_handler_args_to_js(
    JSContextRef ctx, PointerEvent event, ResponderEventType event_type) {
    auto event_type_num =
        static_cast<std::underlying_type_t<ResponderEventType>>(event_type);
    return std::vector<JSValueRef>{pointer_event_to_js(ctx, event),
                                   JSValueMakeNumber(ctx, event_type_num)};
}

void responder_handler_ret_val_from_js(JSContextRef ctx, JSValueRef value){};

JSValueRef responder_elem_get_handler(JSContextRef ctx, JSObjectRef object,
                                      JSStringRef property_name,
                                      JSValueRef* exception) {
    // TODO return something meaningful
    return JSValueMakeUndefined(ctx);
}

bool responder_elem_set_handler(JSContextRef ctx, JSObjectRef object,
                                JSStringRef property_name, JSValueRef value,
                                JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto elem = get_elem<elements::ResponderElement>(ctx, object);
    elem->handler = FunctionWrapper<void, PointerEvent, ResponderEventType>(
        host->ctx,                          // ctx
        value,                              // function
        responder_handler_args_to_js,       // args_to_js
        responder_handler_ret_val_from_js,  // ret_val_from_js
        [host](JSContextRef ctx, JSValueRef ex) {
            host->module_loader->handle_exception(ex);
        });
    elem->change();
    return true;
}

JSClassRef responder_elem_create_class(JSClassRef parent_class) {
    auto definition =
        create_elem_class_definition("ResponderElement", parent_class);
    JSStaticValue static_values[] = {
        {"handler", responder_elem_get_handler, responder_elem_set_handler,
         kJSPropertyAttributeNone},
        {0, 0, 0, 0}};
    definition.staticValues = static_values;
    return JSClassCreate(&definition);
}

//------------------------------------------------------------------------------
// Text
//------------------------------------------------------------------------------

JSValueRef text_element_get_text(JSContextRef ctx, JSObjectRef object,
                                 JSStringRef property_name,
                                 JSValueRef* exception) {
    auto elem = get_elem<elements::Text>(ctx, object);
    return icu_str_to_js(ctx, elem->text);
}

bool text_element_set_text(JSContextRef ctx, JSObjectRef object,
                           JSStringRef propertyName, JSValueRef value,
                           JSValueRef* exception) {
    auto elem = get_elem<elements::Text>(ctx, object);
    elem->text = icu_str_from_js(ctx, value);
    elem->change();
    return true;
}

JSClassRef text_elem_create_class(JSClassRef parent_class) {
    auto definition =
        create_elem_class_definition("TextElement", parent_class);
    JSStaticValue static_values[] = {
        {"text", text_element_get_text, text_element_set_text,
         kJSPropertyAttributeDontDelete},
        {0, 0, 0, 0}};
    definition.staticValues = static_values;
    return JSClassCreate(&definition);
}

//------------------------------------------------------------------------------
// Scroll
//------------------------------------------------------------------------------

bool scroll_elem_set_scroll_top(JSContextRef ctx, JSObjectRef object,
                                       JSStringRef property_name,
                                       JSValueRef value,
                                       JSValueRef* exception) {
    auto elem = get_elem<ScrollElement>(ctx, object);
    elem->scroll_top = int_from_js(ctx, value);
    elem->update_transform();
    return true;
}

JSValueRef scroll_elem_get_scroll_top(JSContextRef ctx,
                                             JSObjectRef object,
                                             JSStringRef property_name,
                                             JSValueRef* exception) {
    auto elem = get_elem<ScrollElement>(ctx, object);
    return int_to_js(ctx, elem->scroll_top);
}

JSClassRef scroll_elem_create_class(JSClassRef parent_class) {
    auto definition =
        create_elem_class_definition("ScrollElement", parent_class);
    JSStaticValue static_values[] = {
        {"scrollTop", scroll_elem_get_scroll_top,
         scroll_elem_set_scroll_top, kJSPropertyAttributeNone},
        {0, 0, 0, 0}};
    definition.staticValues = static_values;
    return JSClassCreate(&definition);
}

//------------------------------------------------------------------------------
// Size
//------------------------------------------------------------------------------

bool sized_elem_set_size_constraints(JSContextRef ctx, JSObjectRef object,
                                       JSStringRef property_name,
                                       JSValueRef value,
                                       JSValueRef* exception) {
    auto elem = get_elem<elements::Sized>(ctx, object);
    elem->size_constraints =
        size_constraints_from_js(ctx, JSValueToObject(ctx, value, nullptr));
    elem->change();
    return true;
}

JSValueRef sized_elem_get_size_constraints(JSContextRef ctx,
                                             JSObjectRef object,
                                             JSStringRef property_name,
                                             JSValueRef* exception) {
    auto elem = get_elem<elements::Sized>(ctx, object);
    return size_constraints_to_js(ctx, elem->size_constraints);
}

JSClassRef sized_elem_create_class(JSClassRef parent_class) {
    auto definition =
        create_elem_class_definition("SizedElement", parent_class);
    JSStaticValue static_values[] = {
        {"sizeConstraints", sized_elem_get_size_constraints,
         sized_elem_set_size_constraints, kJSPropertyAttributeNone},
        {0, 0, 0, 0}};
    definition.staticValues = static_values;
    return JSClassCreate(&definition);
}

JSObjectRef sized_elem_call_as_constructor(JSContextRef ctx,
                                          JSObjectRef constructor,
                                          size_t argument_count,
                                          const JSValueRef arguments[],
                                          JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto size_constrains = elements::SizeConstraints{};
    auto elem = std::make_shared<elements::Sized>(
        std::make_shared<elements::Placeholder>(), size_constrains);
    return host->element_index->create_js_object(elem);
}

//------------------------------------------------------------------------------
// Stack
//------------------------------------------------------------------------------

JSClassRef stack_elem_create_class(JSClassRef parent_class) {
    auto definition =
        create_elem_class_definition("StackElement", parent_class);
    return JSClassCreate(&definition);
}

//------------------------------------------------------------------------------
// Translate
//------------------------------------------------------------------------------

bool translate_elem_set_left(JSContextRef ctx, JSObjectRef object,
                             JSStringRef property_name, JSValueRef value,
                             JSValueRef* exception) {
    auto elem = get_elem<TranslateElement>(ctx, object);
    elem->translation.left = value_from_js(ctx, value);
    elem->change();
    return true;
}

JSValueRef translate_elem_get_left(JSContextRef ctx, JSObjectRef object,
                                   JSStringRef property_name,
                                   JSValueRef* exception) {
    auto elem = get_elem<TranslateElement>(ctx, object);
    return value_to_js(ctx, elem->translation.left);
}

bool translate_elem_set_top(JSContextRef ctx, JSObjectRef object,
                             JSStringRef property_name, JSValueRef value,
                             JSValueRef* exception) {
    auto elem = get_elem<TranslateElement>(ctx, object);
    elem->translation.top = value_from_js(ctx, value);
    elem->change();
    return true;
}

JSValueRef translate_elem_get_top(JSContextRef ctx, JSObjectRef object,
                                   JSStringRef property_name,
                                   JSValueRef* exception) {
    auto elem = get_elem<TranslateElement>(ctx, object);
    return value_to_js(ctx, elem->translation.top);
}

JSClassRef translate_elem_create_class(JSClassRef parent_class) {
    auto definition =
        create_elem_class_definition("TranslateElement", parent_class);
    JSStaticValue static_values[] = {
        {"left", translate_elem_get_left, translate_elem_set_left,
         kJSPropertyAttributeNone},
        {"top", translate_elem_get_top, translate_elem_set_top,
         kJSPropertyAttributeNone},
        {0, 0, 0, 0}};
    definition.staticValues = static_values;
    return JSClassCreate(&definition);
}

}  // namespace aardvark::js
