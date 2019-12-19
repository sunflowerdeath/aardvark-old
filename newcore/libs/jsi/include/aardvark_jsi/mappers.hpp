#pragma once

#include <string>
#include <tl/expected.hpp>

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

    ResType operator()(ArgsTypes... args) {
        auto object = ctx->object_make(nullptr);
        auto js_args = mapper->args_to_js(*ctx, args...);
        auto js_res = function.call_as_function(nullptr /* this */, js_args);
        if (mapper->res_from_js) {
            return mapper->res_from_js(*ctx, js_res);
        }  // else return type is void
    }

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
            // TODO try from js
            res_from_js = [=](Context& ctx, const Value& value) {
                return res_mapper->from_js(ctx, value);
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
        // TODO check is function
        return WrappedFunction(this, &ctx, value.to_object());
    }

  private:
    std::function<std::vector<Value>(Context&, ArgsTypes...)> args_to_js;
    std::function<ResType(Context&, const Value&)> res_from_js;
};

}  // namespace aardvark::jsi

