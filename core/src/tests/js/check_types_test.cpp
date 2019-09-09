#include "Catch2/catch.hpp"
#include <memory>
#include "JavaScriptCore/JavaScript.h"
#include "../../js/helpers.hpp"
#include "../../js/check_types.hpp"

using namespace aardvark::js;

void require_error(const check_types::Checker& checker, JSContextRef ctx,
                   JSValueRef value) {
    auto result = checker(ctx, value, "name", "location");
    REQUIRE(result.has_value());
}

void require_valid(const check_types::Checker& checker, JSContextRef ctx,
                   JSValueRef value) {
    auto result = checker(ctx, value, "name", "location");
    REQUIRE(!result.has_value());
}

TEST_CASE("check_types", "[check_types]" ) {
    auto ctx = JSGlobalContextCreate(nullptr);

    SECTION("primitive types") {
        require_valid(check_types::number, ctx, JSValueMakeNumber(ctx, 21));
        require_error(check_types::number, ctx, JSValueMakeBoolean(ctx, true));
    }

    SECTION("optional") {
        auto checker = check_types::optional(check_types::number);
        require_valid(checker, ctx, JSValueMakeNumber(ctx, 42));
        require_valid(checker, ctx, JSValueMakeNull(ctx));
        require_valid(checker, ctx, JSValueMakeUndefined(ctx));
        require_error(checker, ctx, JSValueMakeBoolean(ctx, true));
    }

    SECTION("array_of") {
        auto checker = check_types::array_of(check_types::number);

        JSValueRef valid_values[] = { JSValueMakeNumber(ctx, 42) };
        auto valid = JSObjectMakeArray(ctx, 1, valid_values, nullptr);
        require_valid(checker, ctx, valid);

        JSValueRef invalid_values[] = { JSValueMakeBoolean(ctx, true) };
        auto invalid = JSObjectMakeArray(ctx, 1, invalid_values, nullptr);
        require_error(checker, ctx, invalid);
    }

    SECTION("object_of") {
        auto checker = check_types::object_of(check_types::number);

        auto prop_name = JsStringWrapper("propName");
        auto valid = JSObjectMake(ctx, nullptr, nullptr);
        JSObjectSetProperty(ctx, valid, prop_name.get(),
                            JSValueMakeNumber(ctx, 1), kJSPropertyAttributeNone,
                            nullptr);
        require_valid(checker, ctx, valid);

        auto invalid = JSObjectMake(ctx, nullptr, nullptr);
        JSObjectSetProperty(ctx, invalid, prop_name.get(),
                            JSValueMakeBoolean(ctx, true),
                            kJSPropertyAttributeNone, nullptr);
        require_error(checker, ctx, invalid);
    }

    SECTION("union") {
        auto checker = check_types::make_union(
            {check_types::number, check_types::boolean});

        auto valid1 = JSValueMakeNumber(ctx, 42);
        auto valid2 = JSValueMakeNumber(ctx, true);
        require_valid(checker, ctx, valid1);
        require_valid(checker, ctx, valid2);

        auto invalid = JSObjectMake(ctx, nullptr, nullptr);
        require_error(checker, ctx, invalid);
    }

    SECTION("enum") {
        auto ctx_sptr = std::make_shared<JSGlobalContextWrapper>(ctx);
        auto valid1 = JSValueMakeNumber(ctx, 42);
        auto valid2 = JSValueMakeNumber(ctx, true);
        auto checker =
            check_types::make_enum({JsValueWrapper(ctx_sptr, valid1),
                                    JsValueWrapper(ctx_sptr, valid2)});

        require_valid(checker, ctx, valid1);
        require_valid(checker, ctx, valid2);

        auto invalid = JSValueMakeNumber(ctx, 23);
        require_error(checker, ctx, invalid);
    }
}
