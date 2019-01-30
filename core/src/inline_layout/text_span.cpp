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
                                     fit_span,       // fit_span
                                     remainder_span  // remainder_span
    );
};

InlineLayoutResult TextSpan::layout(InlineConstraints constraints) {
    auto is_line_start =
        constraints.total_line_width == constraints.remaining_line_width;

    if (linebreak == LineBreak::never) {
        // Wrap if not at the start of the line, otherwise always fit
        auto width = measure_text_width(text, paint);
        auto required_width = constraints.remaining_line_width -
                              constraints.padding_before -
                              constraints.padding_after;
        if (width > required_width && !is_line_start) {
            return InlineLayoutResult::wrap();
        } else {
            return InlineLayoutResult::fit(width,
                                           LineMetrics::from_paint(paint));
        }
    }

    if (linebreak == LineBreak::anywhere) {
        auto width = measure_text_width(text, paint);
        if (width > constraints.remaining_line_width) {
            auto fit_width = 0.0f;
            auto fit_chars = break_text(
                text, paint, constraints.remaining_line_width, &fit_width);
            if (fit_chars == 0) {
                if (!is_line_start) return InlineLayoutResult::wrap();
                // When text cannot be wrapped again, fit at least 1 char
                return split(1, 0);
            }
            return split(fit_chars, fit_width);
        }
        return InlineLayoutResult::fit(width, LineMetrics::from_paint(paint));
    }

    // linebreak normal/overflow
    linebreaker->setText(text);

    // Iterate through break points
    auto first = linebreaker->first();
    auto start = first;
    auto end = linebreaker->next();
    auto next = linebreaker->next();
    auto acc_width = 0.0f;  // Accumulated segments width
    auto segment_width = 0.0f; // Current segment width
    auto paddings_width = 0.0f;
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
        // If first segment does not fit in line, put it into the current line
        // to prevent endless wrap
        acc_width += segment_width;
        start = end;
        end = next;
        // If it was last segment, set end to DONE
        if (linebreaker->next() == BreakIterator::DONE) {
            end = BreakIterator::DONE;
        }
    }

    if (end == BreakIterator::DONE) {
        return InlineLayoutResult::fit(acc_width,
                                       LineMetrics::from_paint(paint));
    } else if (start == linebreaker->first()) {
        return InlineLayoutResult::wrap();
    } else {
       auto fit_text = text.tempSubString(0, start);
       auto remainder_text = text.tempSubString(start);
       auto fit_span = std::make_shared<TextSpan>(fit_text, paint, linebreak,
                                                  SpanBase{this, 0});
       auto remainder_span =
           std::make_shared<TextSpan>(remainder_text, paint, linebreak,
                                      SpanBase{this, fit_text.countChar32()});
       return InlineLayoutResult::split(
           acc_width,                       // width
           LineMetrics::from_paint(paint),  // metrics
           fit_span,                        // fit_span
           remainder_span                   // remainder_span
       );
    }
};

std::shared_ptr<Element> TextSpan::render(
    std::optional<SpanSelectionRange> selection) {
    return std::make_shared<elements::Text>(text, paint);
};

}  // namespace aardvark::inline_layout
