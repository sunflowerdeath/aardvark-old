#pragma once

#include <functional>
#include <map>
#include <memory>
#include <vector>
#include <iostream>

namespace aardvark::jsi {

class Context;
class Object;

class PointerData {
  public:
    virtual ~PointerData() {};
    virtual PointerData* copy() = 0;
};

class Pointer {
  public:
    Pointer(Context* ctx, PointerData* ptr) : ctx(ctx), ptr(ptr){};

    // copy
    Pointer(const Pointer& other) : ctx(other.ctx) { ptr = other.ptr->copy(); }

    Pointer& operator=(const Pointer& other) { return *this = Pointer(other); }

    // move
    Pointer(Pointer&& other) : ctx(other.ctx), ptr(other.ptr) {
        other.ptr = nullptr;
    }

    Pointer& operator=(Pointer&& other) noexcept {
        ctx = other.ctx;
        ptr = std::exchange(other.ptr, nullptr);
        return *this;
    }

    ~Pointer() { delete ptr; }

    Context* ctx;
    PointerData* ptr;
};

class String : public Pointer {
  public:
    using Pointer::Pointer;
    std::string to_utf8();
};

enum class ValueType {
    undefined,
    null,
    boolean,
    number,
    string,
    object,
    symbol
};

class Value : public Pointer {
  public:
    using Pointer::Pointer;

    ValueType get_type() const;
    bool to_bool() const;
    double to_number() const;
    String to_string() const;
    Object to_object() const;
    bool strict_equal_to(const Value& value) const;
};

class Object : public Pointer {
  public:
    using Pointer::Pointer;

    void set_private_data(void* data);
    void* get_private_data();

    template <typename T>
    T get_private_data() {
        return *static_cast<T*>(get_private_data());
    }

    Value to_value();

    Value get_prototype();
    void set_prototype(const Value& prototype);

    std::vector<std::string> get_property_names();
    bool has_property(const std::string& name);
    Value get_property(const std::string& name);
    void set_property(const std::string& name, const Value& value);
    void delete_property(const std::string& name);

    bool is_function();
    Value call_as_function(
        const Value* jsi_this, const std::vector<Value>& jsi_args);

    bool is_constructor();
    Value call_as_constructor(const std::vector<Value>& arguments);

    bool is_array();
    Value get_property_at_index(size_t index);
    void set_property_at_index(size_t index, const Value& value);
};

using Function = std::function<Value(Value&, std::vector<Value>&)>;

using ClassPropertyGetter = std::function<Value(Object&)>;

using ClassPropertySetter = std::function<bool(Object&, Value&)>;

struct ClassPropertyDefinition {
    ClassPropertyGetter get;
    ClassPropertySetter set;
};

using ClassFinalizer = std::function<void(const Object&)>;

struct ClassDefinition {
    std::string name;
    std::map<std::string, Function> methods;
    std::map<std::string, ClassPropertyDefinition> properties;
    ClassFinalizer finalizer;
};

class Class : public Pointer {
  public:
    using Pointer::Pointer;
};

class Script {
    Script();
    ~Script();
};

struct JsErrorLocation {
    std::string source_url;
    int line;
    int column;
};

class JsError : public std::exception {
  public:
    JsError(
        const Value& value, const std::string& message,
        const JsErrorLocation& location)
        : value(value), message(message), location(location){};

    const char* what() const noexcept override {
        return "Uncaught js exception";
    }

    Value value;
    std::string message;
    JsErrorLocation location;
};

class Context {
  public:
    virtual Script create_script(
        const std::string& source, const std::string& source_url) = 0;
    virtual Value eval_script(
        const std::string& script, Object* js_this,
        const std::string& source_url) = 0;
    virtual void garbage_collect() = 0;
    virtual Object get_global_object() = 0;

    // String
    virtual String string_make_from_utf8(const std::string& str) = 0;
    virtual std::string string_to_utf8(const String&) = 0;

    // Value
    virtual Value value_make_bool(bool value) = 0;
    virtual Value value_make_number(double value) = 0;
    virtual Value value_make_null() = 0;
    virtual Value value_make_undefined() = 0;
    virtual Value value_make_string(const String& str) = 0;
    virtual Value value_make_object(const Object& object) = 0;

    virtual ValueType value_get_type(const Value& value) = 0;
    virtual bool value_to_bool(const Value& value) = 0;
    virtual double value_to_number(const Value& value) = 0;
    virtual String value_to_string(const Value& value) = 0;
    virtual Object value_to_object(const Value& value) = 0;

    virtual bool value_strict_equal(const Value& a, const Value& b) = 0;

    // Class
    virtual Class class_create(const ClassDefinition& definition) = 0;

    // Object
    virtual Object object_make(const Class* js_class) = 0;
    virtual Object object_make_function(const Function& function) = 0;
    virtual Object object_make_constructor(const Class& js_class) = 0;
    virtual Object object_make_array() = 0;

    virtual Value object_to_value(const Object& object) = 0;

    virtual void object_set_private_data(const Object& object, void* data) = 0;
    virtual void* object_get_private_data(const Object& object) = 0;

    virtual Value object_get_prototype(const Object& object) = 0;
    virtual void object_set_prototype(
        const Object& object, const Value& prototype) = 0;

    virtual std::vector<std::string> object_get_property_names(
        const Object& object) = 0;
    virtual bool object_has_property(
        const Object& object, const std::string& name) = 0;
    virtual Value object_get_property(
        const Object& object, const std::string& name) = 0;
    virtual void object_delete_property(
        const Object& object, const std::string& name) = 0;
    virtual void object_set_property(
        const Object& object, const std::string& name, const Value& value) = 0;

    virtual bool object_is_function(const Object& object) = 0;
    virtual Value object_call_as_function(
        const Object& object, const Value* jsi_this,
        const std::vector<Value>& jsi_args) = 0;

    virtual bool object_is_constructor(const Object& object) = 0;
    virtual Value object_call_as_constructor(
        const Object& object, const std::vector<Value>& arguments) = 0;

    virtual bool object_is_array(const Object& object) = 0;
    virtual Value object_get_property_at_index(
        const Object& object, size_t index) = 0;
    virtual void object_set_property_at_index(
        const Object& object, size_t index, const Value& value) = 0;
};

}  // namespace aardvark::jsi
