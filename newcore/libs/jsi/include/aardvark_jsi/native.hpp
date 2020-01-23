#pragma once

#include "jsi.hpp"
#include "mappers.hpp"

namespace aardvark::jsi {

template <typename... ArgTypes>
class ArgsMapper {
  public:
    ArgsMapper(
        std::string func_name,
        std::tuple<const char*, Mapper<ArgTypes>*>... arg_defs)
        : func_name(func_name) {
        make_from_js(
            sizeof...(ArgTypes),
            arg_defs...,
            std::index_sequence_for<ArgTypes...>());
    }

    template <size_t... Is>
    void make_from_js(
        size_t arg_def_count,
        std::tuple<const char*, Mapper<ArgTypes>*>... args_defs,
        std::index_sequence<Is...>) {
        args_from_js = [=](
            Context & ctx, std::vector<Value> & args, bool should_check)
            -> tl::expected<std::tuple<ArgTypes...>, std::string> {
            if (should_check && args.size() != arg_def_count) {
                // auto err = fmt::format(
                // "Invalid number of arguments supplied to {}. "
                // "Expected {} arguments, got {}.",
                // target, arg_def_count, args.size());
                // check_types::error_to_exception(err, ctx, exception);
                return tl::make_unexpected("");  // TODO error
            }

            auto mapped_args = std::tuple<ArgTypes...>();
            auto failed = false;
            template_foreach(
                [&](auto& tpl) {
                    if (failed) return;
                    auto& [mapped_arg, index, def] = tpl;
                    auto& [name, mapper] = def;
                    /*
                    if (should_check) {
                        auto arg_err_params = CheckErrorParams{
                            "argument", name, func_name};
                        auto res = mapper->try_from_js(
                            ctx, args[index], arg_err_params);
                        if (res.has_value()) {
                            std::get<index>(mapped_args) = res.value();
                        } else {
                            failed = true;
                        }
                    } else {
                    */
                    *mapped_arg = mapper->from_js(ctx, args[index]);
                    // }
                },
                std::forward_as_tuple(
                    &std::get<Is>(mapped_args), Is, args_defs)...);

            if (failed) {
                return tl::make_unexpected("error");  // TODO error
            } else {
                return mapped_args;
            }
        };
    }

    std::tuple<ArgTypes...> from_js(Context& ctx, std::vector<Value>& args) {
        return args_from_js(ctx, args, false).value();
    }

    tl::expected<std::tuple<ArgTypes...>, std::string> try_from_js(
        Context& ctx, std::vector<Value>& args) {
        return args_from_js(ctx, args, true);
    };

  private:
    std::string func_name;
    std::function<tl::expected<std::tuple<ArgTypes...>, std::string>(
        Context& ctx, std::vector<Value>& args, bool)>
        args_from_js;
};

template <typename ResType, typename ThisType, typename... ArgTypes>
class NativeFunction {
  public:
    NativeFunction(
        Context* ctx,
        std::string name,
        Mapper<ResType>* res_mapper,
        Mapper<ThisType>* this_mapper,
        std::tuple<const char*, Mapper<ArgTypes>*>... arg_defs,
        std::function<ResType(ThisType, ArgTypes...)> func)
        : ctx(ctx),
          func(func),
          res_mapper(res_mapper),
          this_mapper(this_mapper),
          args_mapper(ArgsMapper(name, arg_defs...)) {}

    Result<Value> operator()(Value this_val, std::vector<Value> args) {
        auto mapped_args = args_mapper.try_from_js(*ctx, args);
        if (!mapped_args.has_value()) {
            return ctx->value_make_error(mapped_args.error());
        }

        std::optional<ThisType> mapped_this;
        if constexpr (std::is_same<ThisType, void*>::value) {
            mapped_this = nullptr;
        } else {
            auto err_params = CheckErrorParams{"1", "2", "3"};  // TODO
            auto res = this_mapper->try_from_js(*ctx, this_val, err_params);
            if (!res.has_value()) return ctx->value_make_error(res.error());
            mapped_this = res.value();
        }

        auto mapped_args_with_this = std::tuple_cat(
            std::make_tuple(mapped_this.value()), mapped_args.value());
        auto res = std::apply(func, mapped_args_with_this);
        return res_mapper->to_js(*ctx, res);
    }

  private:
    Context* ctx;
    std::function<ResType(ThisType, ArgTypes...)> func;
    Mapper<ThisType>* this_mapper;
    Mapper<ResType>* res_mapper;
    ArgsMapper<ArgTypes...> args_mapper;
};

template <typename ThisType, typename PropType>
class NativeClassGetter {
  public:
    NativeClassGetter(
        Context* ctx,
        std::string name,
        Mapper<ThisType>* this_mapper,
        Mapper<PropType>* prop_mapper,
        std::function<PropType(ThisType)> func)
        : ctx(ctx),
          func(func),
          this_mapper(this_mapper),
          prop_mapper(prop_mapper) {}

    Result<Value> operator()(Object& this_val) {
        auto err_params = CheckErrorParams{"1", "2", "3"};  // TODO
        auto mapped_this =
            this_mapper->try_from_js(*ctx, this_val.to_value(), err_params);
        if (!mapped_this.value()) {
            //TODO error
        }
        auto res = func(mapped_this.value());
        return prop_mapper->to_js(*ctx, res);
    }

  private:
    Context* ctx;
    std::function<PropType(ThisType)> func;
    Mapper<ThisType>* this_mapper;
    Mapper<PropType>* prop_mapper;
};

template <typename ThisType, typename PropType>
class NativeClassSetter {
  public:
    NativeClassSetter(
        Context* ctx,
        std::string name,
        Mapper<ThisType>* this_mapper,
        Mapper<PropType>* prop_mapper,
        std::function<bool(ThisType, PropType)> func)
        : ctx(ctx),
          func(func),
          this_mapper(this_mapper),
          prop_mapper(prop_mapper) {}

    Result<bool> operator()(Object& this_val, Value& prop_val) {
        auto err_params = CheckErrorParams{"1", "2", "3"};  // TODO
        auto mapped_prop = prop_mapper->try_from_js(*ctx, prop_val, err_params);
        if (!mapped_prop.has_value()) {
            return false; // TODO error
        }
        auto mapped_this =
            this_mapper->try_from_js(*ctx, this_val.to_value(), err_params);
        return std::invoke(func, mapped_this.value(), mapped_prop.value());
    }

  private:
    Context* ctx;
    std::function<bool(ThisType, PropType)> func;
    Mapper<ThisType>* this_mapper;
    Mapper<PropType>* prop_mapper;
};

}  // namespace aardvark::jsi
