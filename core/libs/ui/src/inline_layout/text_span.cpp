#include "inline_layout/text_span.hpp"

#include <iostream>

namespace aardvark::inline_layout {

TextSpan::TextSpan(
    UnicodeString text,
    SkPaint paint,
    LineBreak linebreak,
    std::optional<SpanBase> base_span)
    : text(std::move(text)),
      paint(std::move(paint)),
      linebreak(linebreak),
      Span(base_span) {
    init();
};

void TextSpan::init() {
    this->paint.setTextEncoding(SkPaint::kUTF16_TextEncoding);

    if (this->base_span.span == this) {
        // Create linebreaker
        UErrorCode status = U_ZERO_ERROR;
        linebreaker =
            BreakIterator::createLineInstance(Locale::getUS(), status);
        if (U_FAILURE(status)) {
            std::cout << "Failed to create sentence break iterator. Status = "
                      << u_errorName(status) << std::endl;
            exit(1);
        }
    } else {
        // Reuse linebreaker from the base span
        linebreaker =
            dynamic_cast<TextSpan*>(this->base_span.span)->linebreaker;
    }
}

TextSpan::~TextSpan() {
    if (base_span.span == this) delete linebreaker;
};

InlineLayoutResult TextSpan::fit(float measured_width) {
    return InlineLayoutResult::fit(
        measured_width, LineMetrics::from_paint(paint), shared_from_this());
};

InlineLayoutResult TextSpan::split(int fit_chars, float fit_width) {
    auto fit_text = text.tempSubString(0, fit_chars);
    auto remainder_text = text.tempSubString(fit_chars);
    auto fit_span = std::make_shared<TextSpan>(
        fit_text, paint, linebreak, SpanBase{this, 0});
    auto remainder_span = std::make_shared<TextSpan>(
        remainder_text, paint, linebreak, SpanBase{this, fit_chars});
    return InlineLayoutResult::split(
        fit_width, LineMetrics::from_paint(paint), fit_span, remainder_span);
}

InlineLayoutResult TextSpan::wrap() {
    return InlineLayoutResult::wrap(shared_from_this());
}

// Breaks text segment into lines, taking into account special cases
InlineLayoutResult TextSpan::break_segment(
    const UnicodeString& text,
    const InlineConstraints& constraints,
    bool is_last_segment) {
    auto chars_count = text.countChar32();
    auto required_width =
        constraints.remaining_line_width - constraints.padding_before;
    auto fit_width = 0.0f;
    auto fit_chars = break_text(text, paint, required_width, &fit_width);
    if (is_last_segment && fit_chars == chars_count) {
        // If span fits completely without `padding_after` but does not fit
        // with it, split one char to the next line
        if (fit_width > required_width - constraints.padding_after) {
            return split(
                chars_count - 1,
                measure_text_width(text, paint, chars_count - 1));
        } else {
            return fit(fit_width);
        }
    }
    if (fit_chars == 0) {
        auto at_line_start =
            constraints.total_line_width == constraints.remaining_line_width;
        // If span is at the line start, it should fit at least one char
        // to prevent endless linebreaking, otherwise it should wrap
        if (at_line_start) {
            auto first_char_width = measure_text_width(text, paint, 1);
            return chars_count == 1 ? fit(first_char_width)
                                    : split(1, first_char_width);
        } else {
            return wrap();
        }
    }
    return split(fit_chars, fit_width);
}

InlineLayoutResult TextSpan::layout(InlineConstraints constraints) {
    auto chars_count = text.countChar32();
    if (chars_count == 0) return fit(0);
    auto at_line_start =
        constraints.total_line_width == constraints.remaining_line_width;

    if (linebreak == LineBreak::never) {
        auto required_width = constraints.remaining_line_width -
                              constraints.padding_before -
                              constraints.padding_after;
        auto text_width = measure_text_width(text, paint);
        if (text_width <= required_width) return fit(text_width);
        return at_line_start ? fit(text_width) : wrap();
    }

    if (linebreak == LineBreak::anywhere) {
        return break_segment(text, constraints, true);
    }

    // linebreak == normal || linebreak == overflow
    linebreaker->setText(text);
    auto first = linebreaker->first();
    auto start = first;
    auto end = linebreaker->next();
    auto next = linebreaker->next();
    auto fit_width = 0.0f;
    auto segment_width = 0.0f;
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
        if (fit_width + segment_width + paddings_width >
            constraints.remaining_line_width) {
            break;
        }
        fit_width += segment_width;
        start = end;
        end = next;
        next = linebreaker->next();
    }

    if (at_line_start && start == first && end != BreakIterator::DONE) {
        auto is_last_segment = linebreaker->next() == BreakIterator::DONE;
        if (linebreak == LineBreak::normal) {
            // If span is at the line start, it should fit at least one segment,
            // to prevent endless linebreaking
            return is_last_segment ? fit(segment_width)
                                   : split(end, segment_width);
        } else if (linebreak == LineBreak::overflow) {
            auto segment_text = text.tempSubString(start, end - start);
            return break_segment(segment_text, constraints, true);
        }
    }

    if (end == BreakIterator::DONE) {
        return fit(fit_width);
    } else if (start == linebreaker->first()) {
        return wrap();
    } else {
        return split(start, fit_width);
    }
}

std::shared_ptr<Element> TextSpan::render() {
    return std::make_shared<TextElement>(text, paint);
}

UnicodeString TextSpan::get_text() { return text; }

int TextSpan::get_text_length() { 
    // TODO cache
    return text.countChar32();
}

std::shared_ptr<Span> TextSpan::slice(int start, int end) {
    auto new_text = text.tempSubString(start, end - start + 1);
    auto new_base = SpanBase{base_span.span, base_span.prev_offset + start};
    return std::make_shared<TextSpan>(new_text, paint, linebreak, new_base);
}

int TextSpan::get_text_offset_at_position(int position) {
    // TODO
}

}  // namespace aardvark::inline_layout
