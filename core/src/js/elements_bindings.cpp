#include "elements_bindings.hpp"
#include "../elements/elements.hpp"
#include "bindings_host.hpp"
#include "base_types_bindings.hpp"

namespace aardvark::js {

// Element
std::shared_ptr<Element> get_elem(JSContextRef ctx, JSObjectRef object) {
    return BindingsHost::get(ctx)->element_index->get_native_object(object);
}

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

// TODO
// JSValueRef element_get_children() {}

JSValueRef element_append_child(JSContextRef ctx, JSObjectRef function,
                                JSObjectRef object, size_t argument_count,
                                const JSValueRef arguments[],
                                JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
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
    auto elem = host->element_index->get_native_object(object);
    auto child = host->element_index->get_native_object(
        JSValueToObject(ctx, arguments[0], exception));
    elem->remove_child(child);
    return JSValueMakeUndefined(ctx);
}

// TODO
// JSValueRef element_insert_child_before(

void element_finalize(JSObjectRef object) {
    ObjectsIndex<Element>::remove(object);
}

JSClassRef element_create_class() {
    auto definition = kJSClassDefinitionEmpty;
    JSStaticFunction static_functions[] = {
        {"appendChild", element_append_child, PROP_ATTR_STATIC},
        {"removeChild", element_remove_child, PROP_ATTR_STATIC},
        {0, 0, 0}};
    JSStaticValue static_values[] = {
        {"width", element_get_width, nullptr, PROP_ATTR_STATIC},
        {"height", element_get_height, nullptr, PROP_ATTR_STATIC},
        {"left", element_get_left, nullptr, PROP_ATTR_STATIC},
        {"top", element_get_top, nullptr, PROP_ATTR_STATIC},
        {"parent", element_get_parent, nullptr, PROP_ATTR_STATIC},
        {0, 0, 0, 0}};
    definition.className = "Element";
    definition.finalize = element_finalize;
    definition.staticFunctions = static_functions;
    definition.staticValues = static_values;
    return JSClassCreate(&definition);
}

//------------------------------------------------------------------------------
// Align
//------------------------------------------------------------------------------

elements::Align* get_align_elem(JSContextRef ctx, JSObjectRef object) {
    auto host = BindingsHost::get(ctx);
    auto elem = host->element_index->get_native_object(object);
    return dynamic_cast<elements::Align*>(elem.get());
}

bool align_element_set_align(JSContextRef ctx, JSObjectRef object,
                             JSStringRef property_name, JSValueRef value,
                             JSValueRef* exception) {
    auto align_elem = get_align_elem(ctx, object);
    auto insets = alignment_from_js(ctx, JSValueToObject(ctx, value, nullptr));
    align_elem->insets = insets;
    align_elem->change();
    return true;
}

JSValueRef align_element_get_align(JSContextRef ctx, JSObjectRef object,
                                   JSStringRef property_name,
                                   JSValueRef* exception) {
    auto align_elem = get_align_elem(ctx, object);
    return alignment_to_js(ctx, align_elem->insets);
}

JSClassRef align_elem_create_class(JSClassRef base_class) {
    auto definition = kJSClassDefinitionEmpty;
    JSStaticValue static_values[] = {
        {"align", align_element_get_align, align_element_set_align,
         kJSPropertyAttributeNone},
        {0, 0, 0, 0}};
    definition.className = "AlignElement";
    definition.parentClass = base_class;
    definition.staticValues = static_values;
    return JSClassCreate(&definition);
}

JSObjectRef align_elem_call_as_constructor(JSContextRef ctx,
                                           JSObjectRef constructor,
                                           size_t argumentCount,
                                           const JSValueRef arguments[],
                                           JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto insets = elements::EdgeInsets{};
    auto elem = std::make_shared<elements::Align>(
        std::make_shared<elements::Placeholder>(), insets);
    return host->element_index->create_js_object(elem);
}

//------------------------------------------------------------------------------
// Background
//------------------------------------------------------------------------------

JSValueRef background_element_get_background(JSContextRef ctx,
                                             JSObjectRef object,
                                             JSStringRef property_name,
                                             JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto elem = host->element_index->get_native_object(object);
    auto bg_elem = dynamic_cast<elements::Background*>(elem.get());
    return JSValueMakeNumber(ctx, 123.0);
}

bool background_element_set_background(JSContextRef ctx, JSObjectRef object,
                                       JSStringRef propertyName,
                                       JSValueRef value,
                                       JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto elem = host->element_index->get_native_object(object);
    auto bg_elem = dynamic_cast<elements::Background*>(elem.get());
    bg_elem->color = SK_ColorRED;
    return true;
}

JSClassRef background_elem_create_class(JSClassRef element_class) {
    auto definition = kJSClassDefinitionEmpty;
    JSStaticValue static_values[] = {
        {"background", background_element_get_background,
         background_element_set_background, PROP_ATTR_STATIC},
        {0, 0, 0, 0}};
    definition.className = "BackgroundElement";
    definition.parentClass = element_class;
    definition.staticValues = static_values;
    return JSClassCreate(&definition);
}

JSObjectRef background_elem_call_as_constructor(JSContextRef ctx,
                                                JSObjectRef constructor,
                                                size_t argument_count,
                                                const JSValueRef arguments[],
                                                JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto elem = std::make_shared<elements::Background>(SK_ColorRED);
    return host->element_index->create_js_object(elem);
}

//------------------------------------------------------------------------------
// Stack
//------------------------------------------------------------------------------

JSClassRef stack_elem_create_class(JSClassRef element_class) {
    auto definition = kJSClassDefinitionEmpty;
    definition.className = "Stack";
    definition.parentClass = element_class;
    return JSClassCreate(&definition);
}

JSObjectRef stack_elem_call_as_constructor(JSContextRef ctx,
                                              JSObjectRef constructor,
                                              size_t argument_count,
                                              const JSValueRef arguments[],
                                              JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);
    auto elem = std::make_shared<elements::Stack>();
    return host->element_index->create_js_object(elem);
}

//------------------------------------------------------------------------------
// Text
//------------------------------------------------------------------------------

JSClassRef text_elem_create_class(JSClassRef element_class) {
    auto definition = kJSClassDefinitionEmpty;
    definition.className = "TextElement";
    definition.parentClass = element_class;
    return JSClassCreate(&definition);
}

JSObjectRef text_elem_call_as_constructor(JSContextRef ctx,
                                          JSObjectRef constructor,
                                          size_t argument_count,
                                          const JSValueRef arguments[],
                                          JSValueRef* exception) {
    auto host = BindingsHost::get(ctx);

    auto js_str =
        JSValueToStringCopy(ctx, arguments[0], /* exception */ nullptr);
    auto uni_str = UnicodeString(JSStringGetCharactersPtr(js_str),
                                 JSStringGetLength(js_str));
    // TODO must call JSStringRelease(js_str);

    SkPaint paint;
    paint.setColor(SK_ColorRED);
    paint.setTextSize(32);
    paint.setAntiAlias(true);
    paint.setTextEncoding(SkPaint::kUTF16_TextEncoding);

    auto elem = std::make_shared<elements::Text>(uni_str, paint);

    return host->element_index->create_js_object(elem);
}

}  // namespace aardvark::js
