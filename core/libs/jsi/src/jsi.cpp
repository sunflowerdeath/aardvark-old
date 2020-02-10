#include "jsi.hpp"

namespace aardvark::jsi {

Error::Error(Context* ctx, Value* value) : ctx(ctx) {
    value_ptr = std::make_shared<Value>(*value);
}

Value Error::value() { return *value_ptr.get(); }

std::string Error::message() {
    return value_ptr->to_string().value().to_utf8();
}

std::optional<ErrorLocation> Error::location() {
    return ctx->value_get_error_location(*value_ptr);
    /*
    if (value_ptr->get_type() != ValueType::object) return std::nullopt;
    auto obj = value_ptr->to_object().value();
    if (!obj.has_property("sourceURL")) return std::nullopt;
    auto source_url = obj.get_property("sourceURL")
                          .and_then([](auto val) { return val.to_string(); })
                          .map([](auto val) { return val.to_utf8(); })
                          .value_or("");
    auto line = obj.get_property("line")
                    .and_then([](auto val) { return val.to_number(); })
                    .map([](auto val) { return static_cast<int>(val); })
                    .value_or(-1);
    auto column = obj.get_property("column")
                      .and_then([](auto val) { return val.to_number(); })
                      .map([](auto val) { return static_cast<int>(val); })
                      .value_or(-1);
    return ErrorLocation{source_url, line, column};
    */
}

// String

std::string String::to_utf8() const { return ctx->string_to_utf8(*this); };

// Value

ValueType Value::get_type() const { return ctx->value_get_type(*this); }

Result<bool> Value::to_bool() const { return ctx->value_to_bool(*this); }

Result<double> Value::to_number() const { return ctx->value_to_number(*this); }

Result<String> Value::to_string() const { return ctx->value_to_string(*this); }

Result<Object> Value::to_object() const { return ctx->value_to_object(*this); }

bool Value::strict_equal_to(const Value& value) const {
    return ctx->value_strict_equal(*this, value);
}

bool Value::is_error() const { return ctx->value_is_error(*this); }

WeakValue Value::make_weak() const { return ctx->value_make_weak(*this); }

// WeakValue

Value WeakValue::lock() const { return ctx->weak_value_lock(*this); }

// Object

Value Object::to_value() const { return ctx->object_to_value(*this); }

void Object::set_private_data(void* data) const {
    ctx->object_set_private_data(*this, data);
}

void* Object::get_private_data() const {
    return ctx->object_get_private_data(*this);
}

Result<Value> Object::get_prototype() const {
    return ctx->object_get_prototype(*this);
}

VoidResult Object::set_prototype(const Value& proto) const {
    return ctx->object_set_prototype(*this, proto);
}

std::vector<std::string> Object::get_property_names() const {
    return ctx->object_get_property_names(*this);
}

bool Object::has_property(const std::string& name) const {
    return ctx->object_has_property(*this, name);
}

Result<Value> Object::get_property(const std::string& name) const {
    return ctx->object_get_property(*this, name);
}

VoidResult Object::set_property(
    const std::string& name, const Value& value) const {
    return ctx->object_set_property(*this, name, value);
}

VoidResult Object::delete_property(const std::string& name) const {
    return ctx->object_delete_property(*this, name);
}

bool Object::is_function() const { return ctx->object_is_function(*this); }

Result<Value> Object::call_as_function(
    const Value* js_this, const std::vector<Value>& arguments) const {
    return ctx->object_call_as_function(*this, js_this, arguments);
}

bool Object::is_constructor() const {
    return ctx->object_is_constructor(*this);
}

Result<Object> Object::call_as_constructor(
    const std::vector<Value>& arguments) const {
    return ctx->object_call_as_constructor(*this, arguments);
}

bool Object::is_array() const { return ctx->object_is_array(*this); }

Result<Value> Object::get_property_at_index(size_t index) const {
    return ctx->object_get_property_at_index(*this, index);
}

VoidResult Object::set_property_at_index(
    size_t index, const Value& value) const {
    return ctx->object_set_property_at_index(*this, index, value);
}

tl::unexpected<Error> make_error_result(Context& ctx, std::string message) {
    auto err_val = ctx.value_make_error(message);
    return tl::make_unexpected(Error(&ctx, &err_val));
}

}  // namespace aardvark::jsi
