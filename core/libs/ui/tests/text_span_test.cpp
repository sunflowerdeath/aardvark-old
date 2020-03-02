#include <Catch2/catch.hpp>
#include <aardvark/base_types.hpp>
#include <aardvark/inline_layout/text_span.hpp>
#include <iostream>

using namespace aardvark;

TEST_CASE("inline_layout::TextSpan", "[inline_layout] [text_span]") {
    SECTION("slice") {
        auto text = UnicodeString((UChar*)u"abcdefgh");
        SkPaint paint;
        paint.setTextEncoding(SkPaint::kUTF16_TextEncoding);
        auto span = std::make_shared<inline_layout::TextSpan>(text, paint);

        // ab | cdef | gh
        // 01   2345   67 - original
        //      0123      - slice

        auto slice = span->slice(2, 5);
        REQUIRE(slice->get_text() == "cdef");
        REQUIRE(slice->base_span.span == span.get());
        REQUIRE(slice->base_span.prev_offset == 2);
    }

    auto text = UnicodeString((UChar*)u"Hello, World!");
    SkPaint paint;
    paint.setTextEncoding(SkPaint::kUTF16_TextEncoding);
    auto span = std::make_shared<inline_layout::TextSpan>(text, paint);
    auto text_width = inline_layout::measure_text_width(text, paint);
    auto hello = UnicodeString((UChar*)u"Hello, ");
    auto world = UnicodeString((UChar*)u"World!");
    auto hello_width = inline_layout::measure_text_width(hello, paint);
    auto world_width = inline_layout::measure_text_width(world, paint);

    SECTION("linebreak normal: fit") {
        auto constraints = inline_layout::InlineConstraints{
            1000,  // remaining_width
            1000,  // total_width
            0,     // padding_before
            0      // padding_after
        };
        auto res = span->layout(constraints);
        REQUIRE(res.fit_span.value() == span);
        REQUIRE(res.width == text_width);
        REQUIRE(res.remainder_span == std::nullopt);
    }

    SECTION("linebreak normal: wrap") {
        auto constraints = inline_layout::InlineConstraints{
            0,     // remaining_width
            1000,  // total_width
            0,     // padding_before
            0      // padding_after
        };
        auto res = span->layout(constraints);
        REQUIRE(res.fit_span == std::nullopt);
        REQUIRE(res.remainder_span.value() == span);
    }

    SECTION("linebreak normal: split") {
        auto constraints = inline_layout::InlineConstraints{
            hello_width,  // remaining_width
            1000,         // total_width
            0,            // padding_before
            0             // padding_after
        };
        auto res = span->layout(constraints);
        auto fit_span = (inline_layout::TextSpan*)res.fit_span.value().get();
        auto remainder_span =
            (inline_layout::TextSpan*)res.remainder_span.value().get();
        REQUIRE(fit_span->text == hello);
        REQUIRE(res.width == hello_width);
        REQUIRE(remainder_span->text == world);
    }

    SECTION("linebreak normal: should fit at least one segment") {
        auto hello_span =
            std::make_shared<inline_layout::TextSpan>(hello, paint);
        auto constraints = inline_layout::InlineConstraints{
            hello_width - 10,  // remaining_width
            hello_width - 10,  // total_width
            0,                 // padding_before
            0                  // padding_after
        };
        auto res = hello_span->layout(constraints);
        REQUIRE(res.fit_span.value() == hello_span);
        REQUIRE(res.remainder_span == std::nullopt);
    }

    /*
    SECTION("padding") {
        auto alpha_beta_gamma = UnicodeString((UChar*)u"alpha beta gamma");
        auto alpha = UnicodeString((UChar*)u"alpha ");
        auto beta = UnicodeString((UChar*)u"beta ");
        auto gamma = UnicodeString((UChar*)u"gamma");

        // alpha + beta < size < padding_before + alpha + beta
    }
    */

    auto span_never = std::make_shared<inline_layout::TextSpan>(
        text, paint, inline_layout::LineBreak::never);

    SECTION("linebreak never: wrap") {
        // Result should be `wrap` if span is not at the start of the line
        auto constraints = inline_layout::InlineConstraints{
            hello_width - 1,  // remaining_width
            1000,             // total_width
            0,                // padding_before
            0                 // padding_after
        };
        auto res = span_never->layout(constraints);
        REQUIRE(res.type == inline_layout::InlineLayoutResult::Type::wrap);
    }

    SECTION("linebreak never: fit") {
        // Otherwise result always should be `fit`
        auto constraints = inline_layout::InlineConstraints{
            hello_width - 1,  // remaining_width
            hello_width - 1,  // total_width
            0,                // padding_before
            0                 // padding_after
        };
        auto res = span_never->layout(constraints);
        REQUIRE(res.type == inline_layout::InlineLayoutResult::Type::fit);
    }

    auto span_any = std::make_shared<inline_layout::TextSpan>(
        hello, paint, inline_layout::LineBreak::anywhere);

    SECTION("linebreak anywhere: split") {
        // Span should split before char that exceed remaining line width
        auto hell = UnicodeString((UChar*)u"Hell");
        auto hell_width = inline_layout::measure_text_width(hell, paint);
        auto constraints = inline_layout::InlineConstraints{
            hell_width,  // remaining_width
            hell_width,  // total_width
            0,           // padding_before
            0            // padding_after
        };
        auto res = span_any->layout(constraints);
        REQUIRE(res.type == inline_layout::InlineLayoutResult::Type::split);
        auto fit_span =
            dynamic_cast<inline_layout::TextSpan*>(res.fit_span.value().get());
        REQUIRE(fit_span->text == hell);
        auto remainder_span = dynamic_cast<inline_layout::TextSpan*>(
            res.remainder_span.value().get());
        auto remaining_text = UnicodeString((UChar*)u"o, ");
        REQUIRE(remainder_span->text == remaining_text);
    }

    SECTION("linebreak anywhere: wrap") {
        // Result should be `wrap` when not a single char can fit and span
        // is not at the line start
        auto constraints = inline_layout::InlineConstraints{
            1,  // remaining_width
            2,  // total_width
            0,  // padding_before
            0   // padding_after
        };
        auto res = span_any->layout(constraints);
        REQUIRE(res.type == inline_layout::InlineLayoutResult::Type::wrap);
    }

    SECTION("linebreak anywhere: split 1 char") {
        // If span is at the line start, it should fit at least one char to
        // prevent endless linebreaking
        auto constraints = inline_layout::InlineConstraints{
            1,  // remaining_width
            1,  // total_width
            0,  // padding_before
            0   // padding_after
        };
        auto res = span_any->layout(constraints);
        REQUIRE(res.type == inline_layout::InlineLayoutResult::Type::split);
        auto fit_span =
            dynamic_cast<inline_layout::TextSpan*>(res.fit_span.value().get());
        auto fit_text = UnicodeString((UChar*)u"H");
        REQUIRE(fit_span->text == fit_text);
    };

    SECTION("linebreak anywhere: padding") {
        // When span does not fit with paddings, but would fit without
        // `padding_after`, it should split 1 char to the next line
        auto constraints = inline_layout::InlineConstraints{
            hello_width + 30,  // remaining_width
            hello_width + 30,  // total_width
            20,                // padding_before
            20                 // padding_after
        };
        auto res = span_any->layout(constraints);
        REQUIRE(res.type == inline_layout::InlineLayoutResult::Type::split);
        auto fit_span =
            dynamic_cast<inline_layout::TextSpan*>(res.fit_span.value().get());
        auto fit_text = UnicodeString((UChar*)u"Hello,");
        REQUIRE(fit_span->text == fit_text);
    }

    SECTION("linebreak overflow") {
        auto span_overflow = std::make_shared<inline_layout::TextSpan>(
            text, paint, inline_layout::LineBreak::overflow);

        // When segment fits in line, it should break as normal
        auto normal_constraints = inline_layout::InlineConstraints{
            hello_width + 20,  // remaining_width
            hello_width + 20,  // total_width
            0,                 // padding_before
            0                  // padding_after
        };
        auto normal_result = span_overflow->layout(normal_constraints);
        REQUIRE(
            normal_result.type ==
            inline_layout::InlineLayoutResult::Type::split);
        auto fit_span = dynamic_cast<inline_layout::TextSpan*>(
            normal_result.fit_span.value().get());
        REQUIRE(fit_span->text == hello);

        // When segment does not fit in line, it should split segment
        auto hell = UnicodeString((UChar*)u"Hell");
        auto hell_width = inline_layout::measure_text_width(hell, paint);
        auto overflow_constraints = inline_layout::InlineConstraints{
            hell_width,  // remaining_width
            hell_width,  // total_width
            0,           // padding_before
            0            // padding_after
        };
        auto overflow_result = span_overflow->layout(overflow_constraints);
        REQUIRE(
            overflow_result.type ==
            inline_layout::InlineLayoutResult::Type::split);
        fit_span = dynamic_cast<inline_layout::TextSpan*>(
            overflow_result.fit_span.value().get());
        REQUIRE(fit_span->text == hell);
    }
}
