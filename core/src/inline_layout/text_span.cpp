#include "text_span.hpp"
#include <iostream>

namespace aardvark::inline_layout {

TextSpan::TextSpan(UnicodeString text, SkPaint paint, LineBreak linebreak,
                   std::optional<SpanBase> base_span)
    : text(text), paint(paint), linebreak(linebreak), Span(base_span) {
    this->paint.setTextEncoding(SkPaint::kUTF16_TextEncoding);

    if (base_span != std::nullopt) {
        // Reuse linebreaker from the base span
        linebreaker =
            dynamic_cast<TextSpan*>(base_span.value().span)->linebreaker;
    } else {
        // Create own linebreaker
        UErrorCode status = U_ZERO_ERROR;
        linebreaker =
            BreakIterator::createLineInstance(Locale::getUS(), status);
        if (U_FAILURE(status)) {
            std::cout << "Failed to create sentence break iterator. Status = "
                      << u_errorName(status) << std::endl;
            exit(1);
        }
    }
};

TextSpan::~TextSpan() {
    if (base_span == std::nullopt) delete linebreaker;
};

InlineLayoutResult TextSpan::fit(float measured_width) {
    return InlineLayoutResult::fit(measured_width,
                                   LineMetrics::from_paint(paint));
};

InlineLayoutResult TextSpan::split(int pos, float measured_width) {
    auto fit_text = text.tempSubString(0, pos);
    auto remainder_text = text.tempSubString(pos);
    auto fit_span = std::make_shared<TextSpan>(fit_text, paint, linebreak,
                                               SpanBase{this, 0});
    auto remainder_span =
        std::make_shared<TextSpan>(remainder_text, paint, linebreak,
                                   SpanBase{this, fit_text.countChar32()});
    return InlineLayoutResult::split(measured_width,                  // width
                                     LineMetrics::from_paint(paint),  // metrics
                                     fit_span, remainder_span);
};

InlineLayoutResult TextSpan::layout(InlineConstraints constraints) {
    auto chars_count = text.countChar32();

    if (chars_count == 0) return fit(0);

    auto is_line_start =
        constraints.total_line_width == constraints.remaining_line_width;

    // First, check if span fits completely
    auto required_width = constraints.remaining_line_width -
                          constraints.padding_before -
                          constraints.padding_after;
    auto measured_width = measure_text_width(text, paint);
    if (measured_width <= required_width) return fit(measured_width);

    if (linebreak == LineBreak::never) {
        return is_line_start ? fit(measured_width) : InlineLayoutResult::wrap();
    }

    if (linebreak == LineBreak::anywhere) {
        auto fit_width = 0.0f;
        auto fit_chars = break_text(
            text, paint,
            constraints.remaining_line_width - constraints.padding_before,
            &fit_width);
        if (fit_chars == chars_count) {
            // If span fits completely without `padding_after`, but we already 
            // know that it does not fits with it, split 1 char to the next line
            return split(chars_count - 1, 0);
        }
        if (fit_chars == 0) {
            // If span is at the line start, it should fit at least one char 
            // to prevent endless linebreaking, otherwise it should wrap
            return is_line_start
                       ? (chars_count == 1 ? fit(measured_width) : split(1, 0))
                       : InlineLayoutResult::wrap();
        }
        return split(fit_chars, fit_width);
    }

    // linebreak == normal || linebreak == overflow
    linebreaker->setText(text);
    auto first = linebreaker->first();
    auto start = first;
    auto end = linebreaker->next();
    auto next = linebreaker->next();
    auto acc_width = 0.0f;  // Accumulated segments width
    auto segment_width = 0.0f; // Current segment width
    auto paddings_width = 0.0f;
    // Iterate through break points
    while (end != BreakIterator::DONE) {
        auto substring = text.tempSubString(start, end - start);
        // Line must have space for `padding_before` to fit first segment, and
        // `padding_after` to fit last segment, but they are not counted as own
        // span's width.
        if (start == first) paddings_width += constraints.padding_before;
        if (next == BreakIterator::DONE) {
            paddings_width += constraints.padding_after;
        }
        segment_width = measure_text_width(substring, paint);
        if (acc_width + segment_width + paddings_width >
            constraints.remaining_line_width) {
            break;
        }
        acc_width += segment_width;
        start = end;
        end = next;
        next = linebreaker->next();
    }

    if (start == linebreaker->first() && end != BreakIterator::DONE &&
        is_line_start) {
        if (linebreak == LineBreak::normal) {
            // If span is at the line start, it should fit at least one segment,
            // to prevent endless linebreaking
            acc_width += segment_width;
            start = end;
            end = next;
            // If it was last segment, set end to DONE
            if (linebreaker->next() == BreakIterator::DONE) {
                end = BreakIterator::DONE;
            }
        } else if (linebreak == LineBreak::overflow) {
            auto segment_text = text.tempSubString(start, end - start);
            auto measured_width = 0.0f;
            auto required_width =
                constraints.remaining_line_width - constraints.padding_before;
            auto fit_chars = break_text(segment_text, paint, required_width,
                                        &measured_width);
            start += std::max(fit_chars, 1);
            acc_width += measured_width;
        }
    }

    if (end == BreakIterator::DONE) {
        return fit(acc_width);
    } else if (start == linebreaker->first()) {
        return InlineLayoutResult::wrap();
    } else {
        return split(start, acc_width);
    }
};

std::shared_ptr<Element> TextSpan::render(
    std::optional<SpanSelectionRange> selection) {
    return std::make_shared<elements::Text>(text, paint);
};

}  // namespace aardvark::inline_layout
