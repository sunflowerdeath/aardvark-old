#include "jsi.hpp"

namespace aardvark::jsi {

void string_protect(Context* ctx, void* ptr) { ctx->string_protect(ptr); }
void string_unprotect(Context* ctx, void* ptr) { ctx->string_unprotect(ptr); }

void value_protect(Context* ctx, void* ptr) { ctx->value_protect(ptr); }
void value_unprotect(Context* ctx, void* ptr) { ctx->value_unprotect(ptr); }

void class_protect(Context* ctx, void* ptr) { ctx->class_protect(ptr); }
void class_unprotect(Context* ctx, void* ptr) { ctx->class_unprotect(ptr); }

void object_protect(Context* ctx, void* ptr) { ctx->object_protect(ptr); }
void object_unprotect(Context* ctx, void* ptr) { ctx->object_unprotect(ptr); }

// String

String::String(std::weak_ptr<Context> ctx, void* ptr, bool weak, bool owned)
    : PointerHolder(ctx, ptr, weak, owned){};

std::string String::to_utf8() {
    return get_ctx_or_throw()->string_to_utf8(*this);
};

// Value

// Value::Value() : PointerHolder(std::weak_ptr<Context>(), nullptr, false){};

Value::Value(std::weak_ptr<Context> ctx, void* ptr, bool weak, bool owned)
    : PointerHolder(ctx, ptr, weak, owned){};

ValueType Value::get_type() const {
    return get_ctx_or_throw()->value_get_type(*this);
}

bool Value::to_bool() const { return get_ctx_or_throw()->value_to_bool(*this); }

double Value::to_number() const {
    return get_ctx_or_throw()->value_to_number(*this);
}

String Value::to_string() const {
    return get_ctx_or_throw()->value_to_string(*this);
}

Object Value::to_object() const {
    return get_ctx_or_throw()->value_to_object(*this);
}

bool Value::strict_equal_to(const Value& value) const {
    return get_ctx_or_throw()->value_strict_equal(*this, value);
}

// Class

Class::Class(std::weak_ptr<Context> ctx, void* ptr, bool weak, bool owned)
    : PointerHolder(ctx, ptr, weak, owned) {}

// Object

Object::Object(std::weak_ptr<Context> ctx, void* ptr, bool weak, bool owned)
    : PointerHolder(ctx, ptr, weak, owned) {}

Value Object::to_value() { return get_ctx_or_throw()->object_to_value(*this); }

void Object::set_private_data(void* data) {
    get_ctx_or_throw()->object_set_private_data(*this, data);
}

void* Object::get_private_data() {
    return get_ctx_or_throw()->object_get_private_data(*this);
}

Value Object::get_prototype() {
    return get_ctx_or_throw()->object_get_prototype(*this);
}

void Object::set_prototype(const Value& proto) {
    get_ctx_or_throw()->object_set_prototype(*this, proto);
}

std::vector<std::string> Object::get_property_names() {
    return get_ctx_or_throw()->object_get_property_names(*this);
}

bool Object::has_property(const std::string& name) {
    return get_ctx_or_throw()->object_has_property(*this, name);
}

Value Object::get_property(const std::string& name) {
    return get_ctx_or_throw()->object_get_property(*this, name);
}

void Object::set_property(const std::string& name, const Value& value) {
    get_ctx_or_throw()->object_set_property(*this, name, value);
}

void Object::delete_property(const std::string& name) {
    get_ctx_or_throw()->object_delete_property(*this, name);
}

bool Object::is_function() {
    return get_ctx_or_throw()->object_is_function(*this);
}

Value Object::call_as_function(
    const Value* js_this, const std::vector<Value>& arguments) {
    return get_ctx_or_throw()->object_call_as_function(
        *this, js_this, arguments);
}

bool Object::is_constructor() {
    return get_ctx_or_throw()->object_is_constructor(*this);
}

Value Object::call_as_constructor(const std::vector<Value>& arguments) {
    return get_ctx_or_throw()->object_call_as_constructor(*this, arguments);
}

bool Object::is_array() { return get_ctx_or_throw()->object_is_array(*this); }

Value Object::get_property_at_index(size_t index) {
    return get_ctx_or_throw()->object_get_property_at_index(*this, index);
}

void Object::set_property_at_index(size_t index, const Value& value) {
    get_ctx_or_throw()->object_set_property_at_index(*this, index, value);
}

}  // namespace aardvark::jsi
