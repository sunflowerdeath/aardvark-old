#include "jsi.hpp"

namespace aardvark::jsi {

// String

std::string String::to_utf8() const { return ctx->string_to_utf8(*this); };

// Value

ValueType Value::get_type() const { return ctx->value_get_type(*this); }

bool Value::to_bool() const { return ctx->value_to_bool(*this); }

double Value::to_number() const { return ctx->value_to_number(*this); }

String Value::to_string() const { return ctx->value_to_string(*this); }

Object Value::to_object() const { return ctx->value_to_object(*this); }

bool Value::strict_equal_to(const Value& value) const {
    return ctx->value_strict_equal(*this, value);
}

// Object

Value Object::to_value() const { return ctx->object_to_value(*this); }

void Object::set_private_data(void* data) const {
    ctx->object_set_private_data(*this, data);
}

void* Object::get_private_data() const {
    return ctx->object_get_private_data(*this);
}

Value Object::get_prototype() const { return ctx->object_get_prototype(*this); }

void Object::set_prototype(const Value& proto) const {
    ctx->object_set_prototype(*this, proto);
}

std::vector<std::string> Object::get_property_names() const {
    return ctx->object_get_property_names(*this);
}

bool Object::has_property(const std::string& name) const {
    return ctx->object_has_property(*this, name);
}

Value Object::get_property(const std::string& name) const {
    return ctx->object_get_property(*this, name);
}

void Object::set_property(const std::string& name, const Value& value) const {
    ctx->object_set_property(*this, name, value);
}

void Object::delete_property(const std::string& name) const {
    ctx->object_delete_property(*this, name);
}

bool Object::is_function() const { return ctx->object_is_function(*this); }

Value Object::call_as_function(
    const Value* js_this, const std::vector<Value>& arguments) const {
    return ctx->object_call_as_function(*this, js_this, arguments);
}

bool Object::is_constructor() const {
    return ctx->object_is_constructor(*this);
}

Value Object::call_as_constructor(const std::vector<Value>& arguments) const {
    return ctx->object_call_as_constructor(*this, arguments);
}

bool Object::is_array() const { return ctx->object_is_array(*this); }

Value Object::get_property_at_index(size_t index) const {
    return ctx->object_get_property_at_index(*this, index);
}

void Object::set_property_at_index(size_t index, const Value& value) const {
    ctx->object_set_property_at_index(*this, index, value);
}

}  // namespace aardvark::jsi
