#pragma once

#include <JavaScriptCore/JavaScript.h>

#include <unordered_map>

#include "jsi.hpp"

namespace aardvark::jsi {

class Jsc_Context : public Context {
  public:
    static std::shared_ptr<Jsc_Context> create();
    static Jsc_Context* get(JSContextRef ctx);

    Jsc_Context();
    ~Jsc_Context();

    Value value_from_jsc(JSValueRef ref);
    Object object_from_jsc(JSObjectRef ref);
    String string_from_jsc(JSStringRef ref);
    Class class_from_jsc(JSClassRef ref);

    JSValueRef value_to_jsc(const Value& value);
    JSObjectRef object_to_jsc(const Object& object);
    JSStringRef string_to_jsc(const String& str);
    JSClassRef class_to_jsc(const Class& cls);

    tl::unexpected<Error> error_from_jsc(JSValueRef ref);
    void error_to_jsc(Error& error, JSValueRef* exception);

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

    WeakValue value_make_weak(const Value& value) override;
    Value weak_value_lock(const WeakValue& value) override;

    ValueType value_get_type(const Value& value) override;
    Result<bool> value_to_bool(const Value& value) override;
    Result<double> value_to_number(const Value& value) override;
    Result<String> value_to_string(const Value& value) override;
    Result<Object> value_to_object(const Value& value) override;

    bool value_is_error(const Value& value) override;
    Value value_make_error(const std::string& message) override;
    std::optional<ErrorLocation> value_get_error_location(
        const Value& value) override;

    bool value_strict_equal(const Value& a, const Value& b) override;

    // Class
    Class class_make(const ClassDefinition& definition) override;

    // Object
    Object object_make(const Class* cls) override;
    Object object_make_function(const Function& function) override;
    Object object_make_constructor(const Class& cls) override;
    Object object_make_constructor2(
        const Class& cls, const Function& function) override;
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
    VoidResult object_set_property(
        const Object& object,
        const std::string& name,
        const Value& value) override;
    VoidResult object_delete_property(
        const Object& object, const std::string& name) override;

    bool object_is_function(const Object& obj) override;
    Result<Value> object_call_as_function(
        const Object& object,
        const Value* this_val,
        const std::vector<Value>& args) override;

    bool object_is_constructor(const Object& object) override;
    Result<Object> object_call_as_constructor(
        const Object& object, const std::vector<Value>& args) override;

    bool object_is_array(const Object& object) override;
    Result<Value> object_get_property_at_index(
        const Object& object, size_t index) override;
    VoidResult object_set_property_at_index(
        const Object& object, size_t index, const Value& value) override;

    JSGlobalContextRef ctx;
    std::optional<Object> error_constructor;
    bool ctx_invalid = false;
    std::unordered_map<JSClassRef, ClassDefinition> class_definitions;

    struct ClassInstanceRecord {
        Jsc_Context* ctx;
        ClassDefinition* definition;
    };

    // When object is created using `object_make`, or constructor created
    // using `object_make_constructor`, this map stores what class it has and
    // what context it belongs. This is needed to implement class properties and
    // finalizers.
    static std::unordered_map<JSObjectRef, ClassInstanceRecord> class_instances;

    static ClassDefinition* get_class_definition(JSObjectRef object);
    static void finalize_class_instance(JSObjectRef object);
};

}  // namespace aardvark::jsi
