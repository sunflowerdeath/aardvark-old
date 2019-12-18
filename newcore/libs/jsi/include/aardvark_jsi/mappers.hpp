#pragma once

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

// template <typename T>
// using ToJsCallback = Value (*)(const Context&, const T&);

// template <typename T>
// using FromJsCallback = T (*)(const Context&, const Value&);

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

}  // namespace aardvark::js

