#pragma once

#include <quickjs/quickjs.h>

#include "jsi.hpp"

namespace aardvark::jsi {

class Qjs_Context : public Context {
  public:
    static std::shared_ptr<Qjs_Context> create();
    static Qjs_Context* get(JSContext* ctx);
    static JSClassID function_class_id;

    Qjs_Context();
    void init();
    ~Qjs_Context();

    // Helpers
    Value value_from_qjs(const JSValue& value, bool finalizing = false);
    Object object_from_qjs(const JSValue& value, bool finalizing = false);

    JSValue value_get_qjs(const Value& value);
    JSValue object_get_qjs(const Object& object);
    std::string string_get_str(const String& str);
    JSClassID class_get_qjs(const Class& cls);

    tl::unexpected<Error> get_error();

    // Global
    Script create_script(
        const std::string& source, const std::string& source_url) override;
    Result<Value> eval(
        const std::string& source,
        Object* this_obj,
        const std::string& source_url) override;
    void garbage_collect() override;
    Object get_global_object() override;

    // String
    String string_make_from_utf8(const std::string& str) override;
    std::string string_to_utf8(const String&) override;

    // Value
    Value value_make_bool(bool value) override;
    Value value_make_number(double value) override;
    Value value_make_null() override;
    Value value_make_undefined() override;
    Value value_make_string(const String& str) override;
    Value value_make_object(const Object& object) override;

    ValueType value_get_type(const Value& value) override;
    Result<bool> value_to_bool(const Value& value) override;
    Result<double> value_to_number(const Value& value) override;
    Result<String> value_to_string(const Value& value) override;
    Result<Object> value_to_object(const Value& value) override;

    bool value_strict_equal(const Value& a, const Value& b) override;

    Value value_make_error(const std::string& message) override;
    bool value_is_error(const Value& value) override;

    // Class
    Class class_make(const ClassDefinition& definition) override;

    // Object
    Object object_make(const Class* js_class) override;
    Object object_make_function(const Function& function) override;
    Object object_make_constructor(const Class& js_class) override;
    Object object_make_array() override;

    Value object_to_value(const Object& object) override;

    void object_set_private_data(const Object& object, void* data) override;
    void* object_get_private_data(const Object& object) override;

    Result<Value> object_get_prototype(const Object& object) override;
    VoidResult object_set_prototype(
        const Object& object, const Value& prototype) override;

    std::vector<std::string> object_get_property_names(
        const Object& object) override;
    bool object_has_property(
        const Object& object, const std::string& name) override;
    Result<Value> object_get_property(
        const Object& object, const std::string& name) override;
    VoidResult object_delete_property(
        const Object& object, const std::string& name) override;
    VoidResult object_set_property(
        const Object& object,
        const std::string& name,
        const Value& value) override;

    bool object_is_function(const Object& object) override;
    Result<Value> object_call_as_function(
        const Object& object,
        const Value* jsi_this,
        const std::vector<Value>& jsi_args) override;

    bool object_is_constructor(const Object& object) override;
    Result<Object> object_call_as_constructor(
        const Object& object, const std::vector<Value>& arguments) override;

    bool object_is_array(const Object& object) override;
    Result<Value> object_get_property_at_index(
        const Object& object, size_t index) override;
    VoidResult object_set_property_at_index(
        const Object& object, size_t index, const Value& value) override;

    JSRuntime* rt;
    JSContext* ctx;
    std::optional<Object> strict_equal_function;
    std::unordered_map<JSClassID, ClassFinalizer> class_finalizers;

    struct ClassInstanceRecord {
        Qjs_Context* ctx;
        JSClassID class_id;
    };

    // JSValue cannot be map key, so need to use pointer
    static std::unordered_map<void*, ClassInstanceRecord> class_instances;
};

}  // namespace aardvark::jsi
