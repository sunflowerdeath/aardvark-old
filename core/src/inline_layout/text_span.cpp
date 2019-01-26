#include "text_span.hpp"
#include <iostream>

namespace aardvark::inline_layout {

TextSpan::TextSpan(UnicodeString text, SkPaint paint,
                   std::optional<SpanBase> base_span)
    : text(text), paint(paint), Span(base_span) {
    this->paint.setTextEncoding(SkPaint::kUTF16_TextEncoding);
    // Create linebreaker
    UErrorCode status = U_ZERO_ERROR;
    linebreaker = BreakIterator::createLineInstance(Locale::getUS(), status);
    if (U_FAILURE(status)) {
        std::cout << "Failed to create sentence break iterator. Status = "
                  << u_errorName(status) << std::endl;
        exit(1);
    }
};

InlineLayoutResult TextSpan::layout(InlineConstraints constraints) {
    linebreaker->setText(text);

    // Iterate through break points
    auto acc_width = 0.0f;  // Accumulated width
    auto segment_width = 0.0f;
    auto start = linebreaker->first();
    auto end = linebreaker->next();
    while (end != BreakIterator::DONE) {
        auto substring = text.tempSubString(start, end - start);
        segment_width = measure_text_width(substring, paint);
        if (segment_width + acc_width > constraints.remaining_line_width) break;
        acc_width += segment_width;
        start = end;
        end = linebreaker->next();
    }

    if (start == linebreaker->first() && end != BreakIterator::DONE &&
        constraints.total_line_width == constraints.remaining_line_width) {
        // If first segment at the start of line did not fit, put it into the 
        // current line to prevent endless linebreaking
        acc_width += segment_width;
        start = end;
        end = linebreaker->next();
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
           std::make_shared<TextSpan>(fit_text, paint,
                                      SpanBase{/* base_span */ this,
                                               /* prev_offset */ 0});
       auto remainder_span = std::make_shared<TextSpan>(
           remainder_text, paint,
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
    std::optional<SpanSelection> selection) {
    return std::make_shared<elements::Text>(text, paint);
};

}  // namespace aardvark::inline_layout
