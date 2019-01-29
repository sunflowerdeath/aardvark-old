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

InlineLayoutResult TextSpan::layout(InlineConstraints constraints) {
    auto at_line_beginning =
        constraints.total_line_width != constraints.remaining_line_width;

    if (linebreak == LineBreak::never) {
        auto width = measure_text_width(text, paint);
        if (width > constraints.remaining_line_width && !at_line_beginning) {
            return InlineLayoutResult::wrap();
        }
        return InlineLayoutResult::fit(width, LineMetrics::from_paint(paint));
    }

    /*
    TODO
    if (linebreak == LineBreak::anywhere) {
        auto width = measure_text_width(text, paint);
        if (width <= constraints.remaining_line_width) {
            return InlineLayoutResult::fit(width,
                                           LineMetrics::from_paint(paint));
        } else {
            auto fit_n = break_text(text, constraints.remaining_line_width);
            auto fit_bytes =
                paint.breakText(text.getBuffer(), text.length() * 2,
                                constraints.remaining_line_width);
            if (fit_bytes == 0) {
                if (!at_line_beginning) return InlineLayoutResult::wrap()
                // Require at least one letter
            } else {
                return InlineLayout::split();
            }
        }
    }
    */

    // linebreak default
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
        at_line_beginning) {
        // If first segment at the start of line did not fit, put it into the 
        // current line to prevent endless linebreaking
        acc_width += segment_width;
        start = end;
        end = next;
        // If it was last segment, set end to DONE
        if (linebreaker->next() == BreakIterator::DONE) {
            end = BreakIterator::DONE;
        }
    }

    if (end == BreakIterator::DONE) {
        return InlineLayoutResult::fit(
            /* width */ acc_width,
            /* metrics */ LineMetrics::from_paint(paint));
    } else if (start == linebreaker->first()) {
        return InlineLayoutResult::wrap();
    } else {
       auto fit_text = text.tempSubString(0, start);
       auto remainder_text = text.tempSubString(start);
       auto fit_span =
           std::make_shared<TextSpan>(fit_text, paint, linebreak,
                                      SpanBase{/* base_span */ this,
                                               /* prev_offset */ 0});
       auto remainder_span = std::make_shared<TextSpan>(
           remainder_text, paint, linebreak,
           SpanBase{/* base_span */ this,
                    /* prev_offset */ fit_text.countChar32()});
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

// TODO destroy linebreaker

}  // namespace aardvark::inline_layout
