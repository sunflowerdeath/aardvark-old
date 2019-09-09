#include "Catch2/catch.hpp"
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
}
