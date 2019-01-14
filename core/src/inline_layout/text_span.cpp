#include "text_span.hpp"
#include <iostream>

namespace aardvark::inline_layout {

TextSpan::TextSpan(UnicodeString text, SkPaint paint,
                   std::optional<SpanBase> base_span)
    : text(text), paint(paint), Span(base_span) {
    paint.setTextEncoding(SkPaint::kUTF16_TextEncoding);
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
    auto acc_width = 0;  // Accumulated width
    auto start = linebreaker->first();
    auto end = linebreaker->next();
    while (end != BreakIterator::DONE) {
        auto substring = text.tempSubString(start, end - start);
        auto text_width = measure_text(substring, paint);
        if (text_width + acc_width > constraints.remaining_line_width) break;
        acc_width += text_width;
        start = end;
        end = linebreaker->next();
    }

    SkPaint::FontMetrics metrics;
    (void)paint.getFontMetrics(&metrics);
    auto line_height = static_cast<int>(metrics.fAscent + metrics.fDescent);
    auto baseline = static_cast<int>(metrics.fAscent);

    if (end == BreakIterator::DONE) {
        // All text fit in the current line
        fit_span.reset();
        remainder_span.reset();
        return InlineLayoutResult{
            this,                          // fit_span
            Size{acc_width, line_height},  // size
            baseline,                      // baseline
            std::nullopt                   // fit_span
        };
    } else if (start == linebreaker->first()) {
        // Nothing did fit
        fit_span.reset();
        remainder_span.reset();
        return InlineLayoutResult{
            std::nullopt,  // fit_span
            Size{0, 0},    // size
            0,             // baseline
            this           // fit_span
        };
    } else {
        auto fit_text = text.tempSubString(0, start);
        auto remainder_text = text.tempSubString(start);
        fit_span = std::make_unique<TextSpan>(fit_text, paint,
                                              SpanBase{/* base_span */ this,
                                                       /* prev_offset */ 0});
        remainder_span = std::make_unique<TextSpan>(
            remainder_text, paint,
            SpanBase{/* base_span */ this,
                     /* prev_offset */ fit_text.countChar32()});
        return InlineLayoutResult{fit_span.get(), Size{acc_width, line_height},
                                  baseline, remainder_span.get()};
    }
};

}  // namespace aardvark::inline_layout
