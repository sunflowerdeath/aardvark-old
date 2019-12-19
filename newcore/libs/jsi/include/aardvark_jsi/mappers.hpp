#pragma once

#include <fmt/format.h>

#include <string>
#include <tl/expected.hpp>
#include <unordered_map>
#include <unordered_set>
#include <variant>

#include "check.hpp"
#include "jsi.hpp"

namespace aardvark::jsi {

template <typename T>
class Mapper {
  public:
    virtual Value to_js(Context& ctx, const T& value) = 0;
    virtual T from_js(Context& ctx, const Value& value) = 0;
    virtual tl::expected<T, std::string> try_from_js(
        Context& ctx, const Value& value,
        const CheckErrorParams& err_params) = 0;
};

template <typename T>
using ToJsCallback = std::function<Value(Context&, const T&)>;

template <typename T>
using FromJsCallback = std::function<T(Context&, const Value&)>;

template <typename T>
class SimpleMapper : public Mapper<T> {
  public:
    SimpleMapper(
        ToJsCallback<T> to_js_cb, FromJsCallback<T> from_js_cb,
        Checker* checker)
        : to_js_cb(to_js_cb), from_js_cb(from_js_cb), checker(checker){};

    Value to_js(Context& ctx, const T& value) override {
        return to_js_cb(ctx, value);
    }

    T from_js(Context& ctx, const Value& value) override {
        return from_js_cb(ctx, value);
    }

    tl::expected<T, std::string> try_from_js(
        Context& ctx, const Value& value,
        const CheckErrorParams& err_params) override {
        auto err = (*checker)(ctx, value, err_params);
        if (err.has_value()) return tl::make_unexpected(err.value());
        return from_js(ctx, value);
    }

  private:
    ToJsCallback<T> to_js_cb;
    FromJsCallback<T> from_js_cb;
    Checker* checker;
};

extern Mapper<bool>* bool_mapper;
extern Mapper<double>* number_mapper;
extern Mapper<int>* int_mapper;
extern Mapper<std::string>* string_mapper;

template <typename T>
class EnumMapper : public Mapper<T> {
    using UnderlyingMapper = Mapper<std::underlying_type_t<T>>;

  public:
    EnumMapper(UnderlyingMapper* mapper) : mapper(mapper){};

    Value to_js(Context& ctx, const T& value) override {
        return mapper->to_js(
            ctx, static_cast<std::underlying_type_t<T>>(value));
    }

    T from_js(Context& ctx, const Value& value) override {
        return static_cast<T>(mapper->from_js(ctx, value));
    }

    tl::expected<T, std::string> try_from_js(
        Context& ctx, const Value& value,
        const CheckErrorParams& err_params) override {
        auto res = mapper->try_from_js(ctx, value, err_params);
        return res.map([](auto value) { return static_cast<T>(value); });
    }

  private:
    UnderlyingMapper* mapper;
};

template <typename F, typename... Ts>
inline void template_foreach(F f, const Ts&... args) {
    [](...) {}((f(args), 0)...);
}

template <typename T, typename... F>
class StructMapper : public Mapper<T> {
  public:
    StructMapper(std::tuple<const char*, F T::*, Mapper<F>*>... fields) {
        template_foreach(
            [&](const auto& arg) { prop_names.push_back(std::get<0>(arg)); },
            fields...);

        // Create functions that iterate over all property definitions and map
        // js properties with corresponding object members using mappers
        map_props_to_js = [=](Context& ctx, const T& value) {
            auto result = ctx.object_make(nullptr);
            template_foreach(
                [&](const auto& field) {
                    auto [prop_name, member_ptr, mapper] = field;
                    auto prop_value = mapper->to_js(ctx, value.*member_ptr);
                    result.set_property(prop_name, prop_value);
                },
                fields...);
            return result.to_value();
        };

        map_props_from_js = [=](Context& ctx, const Value& value,
                                const CheckErrorParams* err_params)
            -> tl::expected<T, std::string> {
            auto should_check = err_params != nullptr;

            if (should_check) {
                auto err = object_checker(ctx, value, *err_params);
                if (err.has_value()) return tl::make_unexpected(err.value());
            }
            auto object = value.to_object();

            T mapped_struct;
            auto failed = false;
            auto error = std::string();
            template_foreach(
                [&](const auto& field) {
                    if (failed) return;
                    auto [prop_name, member_ptr, mapper] = field;
                    auto prop_value = object.has_property(prop_name)
                                          ? object.get_property(prop_name)
                                          : ctx.value_make_undefined();
                    if (should_check) {
                        auto prop_err_params =
                            CheckErrorParams{err_params->kind,
                                             err_params->name + "." + prop_name,
                                             err_params->target};
                        auto res = mapper->try_from_js(
                            ctx, prop_value, prop_err_params);
                        if (res.has_value()) {
                            mapped_struct.*member_ptr = res.value();
                        } else {
                            failed = true;
                            error = res.error();
                        }
                    } else {
                        if (object.has_property(prop_name)) {
                            mapped_struct.*member_ptr =
                                mapper->from_js(ctx, prop_value);
                        }
                    }
                },
                fields...);

            if (failed) {
                return tl::make_unexpected(error);
            } else {
                return mapped_struct;
            }
        };
    }

    Value to_js(Context& ctx, const T& value) override {
        return map_props_to_js(ctx, value);
    }

    T from_js(Context& ctx, const Value& value) override {
        return map_props_from_js(ctx, value, nullptr).value();
    }

    tl::expected<T, std::string> try_from_js(
        Context& ctx, const Value& value,
        const CheckErrorParams& err_params) override {
        return map_props_from_js(ctx, value, &err_params);
    };

  private:
    std::vector<const char*> prop_names;
    std::function<Value(Context& ctx, const T& value)> map_props_to_js;
    std::function<tl::expected<T, std::string>(
        Context& ctx, const Value& value, const CheckErrorParams* err_params)>
        map_props_from_js;
};

template <class ResType, class... ArgsTypes>
class FunctionMapper;

template <class ResType, class... ArgsTypes>
class WrappedFunction {
  public:
    WrappedFunction(
        FunctionMapper<ResType, ArgsTypes...>* mapper, Context* ctx,
        const Object& function)
        : mapper(mapper), ctx(ctx), function(function) {}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
    ResType operator()(ArgsTypes... args) {
        auto object = ctx->object_make(nullptr);
        auto js_args = mapper->args_to_js(*ctx, args...);
        auto js_res = function.call_as_function(nullptr /* this */, js_args);
        if (mapper->res_from_js) {
            return mapper->res_from_js(*ctx, js_res);
        }  // else return type is void
    }
#pragma GCC diagnostic pop

  private:
    FunctionMapper<ResType, ArgsTypes...>* mapper;
    Context* ctx;
    Object function;
};

template <class ResType, class... ArgsTypes>
class FunctionMapper : public Mapper<std::function<ResType(ArgsTypes...)>> {
    friend WrappedFunction<ResType, ArgsTypes...>;
    using FunctionType = std::function<ResType(ArgsTypes...)>;

  public:
    FunctionMapper(
        Mapper<ResType>* res_mapper = nullptr,
        Mapper<ArgsTypes>*... args_mappers) {
        args_to_js = [=](Context& ctx, ArgsTypes... args) {
            auto res = std::vector<Value>();
            template_foreach(
                [&](auto& tpl) {
                    auto& [arg, arg_mapper] = tpl;
                    res.push_back(arg_mapper->to_js(ctx, arg));
                },
                std::forward_as_tuple(args, args_mappers)...);
            return res;
        };
        if (res_mapper != nullptr) {
            // TODO error message
            err_params = CheckErrorParams{"return value", "", "FunctionName"};
            res_from_js = [=](Context& ctx, const Value& value) {
                auto res = res_mapper->try_from_js(ctx, value, err_params);
                if (res.has_value()) return res.value();
                // TODO value_make_error
                throw JsError(
                    ctx.value_make_string(
                        ctx.string_make_from_utf8(res.error())),  // value
                    res.error(),                                  // message
                    JsErrorLocation{"url", 0, 0}                  // location
                );
            };
        }
    }

    Value to_js(Context& ctx, const std::function<ResType(ArgsTypes...)>& value)
        override {
        return ctx.value_make_undefined();
    }

    FunctionType from_js(Context& ctx, const Value& value) override {
        return WrappedFunction(this, &ctx, value.to_object());
    }

    tl::expected<FunctionType, std::string> try_from_js(
        Context& ctx, const Value& value,
        const CheckErrorParams& err_params) override {
        auto res = function_checker(ctx, value, err_params);
        if (res.has_value()) return tl::make_unexpected(res.value());
        return WrappedFunction(this, &ctx, value.to_object());
    }

  private:
    std::function<std::vector<Value>(Context&, ArgsTypes...)> args_to_js;
    std::function<ResType(Context&, const Value&)> res_from_js;
    CheckErrorParams err_params;
};

template <class T>
class ObjectsMapper : Mapper<std::shared_ptr<T>> {
    using ClassGetter = std::function<Class(T*)>;

  public:
    ObjectsMapper(
        std::string type_name, std::variant<Class, ClassGetter> js_class)
        : type_name(type_name), js_class(js_class){};

    Value to_js(
        Context& ctx, const std::shared_ptr<T>& native_object) override {
        auto it = records_map.find(native_object.get());
        if (it != records_map.end()) {
            return it->second.js_value;
        } else {
            return create_js_value(ctx, native_object);
        }
    }

    std::shared_ptr<T> from_js(Context& ctx, const Value& value) override {
        auto record = get_record(value);
        if (records_set.find(record) == records_set.end()) return nullptr;
        return record->native_object;
    }

    tl::expected<std::shared_ptr<T>, std::string> try_from_js(
        Context& ctx, const Value& value,
        const CheckErrorParams& err_params) override {
        auto native_object = from_js(ctx, value);
        if (native_object == nullptr) {
            auto error = fmt::format(
                "Invalid {} `{}` supplied to `{}`, expected `{}`.",
                err_params.kind, err_params.name, err_params.target, type_name);
            return tl::make_unexpected(error);
        } else {
            return native_object;
        }
    }

    static void finalize(const Value& value) {
        auto record = get_record(value);
        if (record == nullptr) return;
        auto index = record->index;
        index->records_map.erase(record->native_object.get());
        index->records_set.erase(record);
    }

  private:
    struct Record {
        std::shared_ptr<T> native_object;
        Value js_value;
        ObjectsMapper<T>* index;
    };

    std::string type_name;
    std::variant<Class, ClassGetter> js_class;
    std::unordered_map<T*, Record> records_map;
    std::unordered_set<Record*> records_set;

    Value create_js_value(
        Context& ctx, const std::shared_ptr<T>& native_object) {
        auto ptr = native_object.get();
        auto the_js_class = std::holds_alternative<Class>(js_class)
                                ? std::get<Class>(js_class)
                                : std::get<ClassGetter>(js_class)(ptr);
        auto js_object = ctx.object_make(&the_js_class);
        auto js_value = js_object.to_value();
        auto res =
            records_map.emplace(ptr, Record{native_object, js_value, this});
        auto record_ptr = &(res.first->second);
        records_set.insert(record_ptr);
        js_object.set_private_data(static_cast<void*>(record_ptr));
        return js_value;
    }

    static Record* get_record(const Value& value) {
        return static_cast<Record*>(value.to_object().get_private_data());
    }
};

}  // namespace aardvark::jsi

