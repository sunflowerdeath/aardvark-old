#pragma once

#include "jsi.hpp"

namespace aardvark::jsi {

class Qjs_Context : public Context {
  public:
	static std::shared_ptr<Qjs_Context> create();

	Script create_script(
		const std::string& source, const std::string& source_url) override;
	Value eval_script(const Script& script) override;
	void garbage_collect() override;
	Object get_global_object() override;

	// String
	String string_make_from_utf8(const std::string& str) override;
	std::string string_to_utf8(const String&) override;
	void string_protect(const String& str) override;
	void string_unprotect(const String& str) override;

	// Value
	Value value_make_bool(bool value) override;
	Value value_make_number(float value) override;
	Value value_make_null() override;
	Value value_make_undefined() override;
	Value value_make_string(const String& str) override;
	Value value_make_object(const Object& object) override;

	void value_protect(const Value& value) override;
	void value_unprotect(const Value& value) override;
	
	ValueType get_type(const Value& value) override;
	bool value_to_bool(const Value& value) override;
	float to_number(const Value& value) override;
	String to_string(const Value& value) override;
	Object to_object(const Value& value) override;

	// Class
	Class class_create(const ClassDefinition& definition) override;

	// Object
	Object object_make(const Class& js_class) override;
	Object object_make_function(const Function& function) override;
	Object object_make_constructor(const Class& js_class) override;
	Object object_make_array() override;

	void object_protect(const Object& object) override;
	void object_retain(const Object& object) override;

	void object_set_private_data(void* data) override;
	void* object_get_private_data() override;

	Value object_get_prototype() override;
	void object_set_prototype(const Value& prototype) override;

	std::vector<std::string> object_get_property_names() override;
	Value object_has_property(const std::string& name) override;
	Value object_get_property_value(const std::string& name) override;
	void object_delete_property(const std::string& name) override;
	void object_set_property(
		const std::string& name, const Value& value) override;

	bool object_is_function() override;
	Value object_call_as_function(
		const Value js_this, const std::vector<Value> arguments) override;

	bool object_is_constructor() override;
	Value object_call_as_constructor(
		const std::vector<Value> arguments) override;
	
	bool object_is_array() override;
	Value object_get_value_at_index(size_t index) override;
  	void object_set_value_at_index(
		  size_t index, const Value& value) override;
  
  private:
	Jsc_Context();
	~Jsc_Context();

	JS_Runtime* rt;
	JS_Context* ctx;
};

} // namespace aardvark::jsi