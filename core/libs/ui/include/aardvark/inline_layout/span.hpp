#pragma once

#include <unicode/unistr.h>

#include <memory>
#include <optional>

#include "../base_types.hpp"
#include "../element.hpp"
#include "line_metrics.hpp"

namespace aardvark::inline_layout {

class Span;

struct InlineConstraints {
    float remaining_line_width;
    float total_line_width;
    float padding_before;
    float padding_after;
};

namespace vert_align {

// Span is aligned on the baseline
float baseline(LineMetrics line, LineMetrics span);

// Span is aligned at the top of the line
float top(LineMetrics line, LineMetrics span);

float center(LineMetrics line, LineMetrics span);

// Span is aligned at the bottom of the line
float bottom(LineMetrics line, LineMetrics span);

// Middle of the span is aligned with middle of x-height of the line
int text_center(LineMetrics line, LineMetrics span);

}  // namespace vert_align

struct InlineLayoutResult {
    enum class Type { fit, split, wrap };

    Type type;
    float width;
    LineMetrics metrics;
    std::optional<std::shared_ptr<Span>> fit_span = std::nullopt;
    std::optional<std::shared_ptr<Span>> remainder_span = std::nullopt;

    static InlineLayoutResult fit(
        float width, LineMetrics metrics, std::shared_ptr<Span> fit_span);
    static InlineLayoutResult split(
        float width,
        LineMetrics metrics,
        std::shared_ptr<Span> fit_span,
        std::shared_ptr<Span> remainder_span);
    static InlineLayoutResult wrap(std::shared_ptr<Span> remainder_span);
};

struct SpanBase {
    Span* span;
    int prev_offset;
};

// Span represents part of the contents of the inline container element
class Span : public std::enable_shared_from_this<Span> {
  public:
    Span(std::optional<SpanBase> base_span = std::nullopt)
        : base_span(base_span){};

    virtual ~Span() = default;

    // Defines how the content should be lay out by the container
    virtual InlineLayoutResult layout(InlineConstraints constraints) = 0;

    // Returns elements that represent this span in the document
    virtual std::shared_ptr<Element> render() = 0;

    // Calculates position of the span in the line, default is baseline
    virtual float vert_align(LineMetrics line, LineMetrics span) {
        return vert_align::baseline(line, span);
    };

    // Returns text content of the span
    virtual UnicodeString get_text() { return UnicodeString(); }

    // Returns length of the span's text
    virtual int get_text_length() { return 0; }

    // Slices new span at provided text offsets
    virtual std::shared_ptr<Span> slice(int start, int end) {
        return shared_from_this();
    }

    // Finds closest text offset at the provided position
    virtual int get_text_offset_at_position(int position) { return 0; }

    // Returns original span and text offset in it
    std::pair<Span*, int> get_original_text_offset(int offset) {
        auto orig_span = this;
        auto orig_offset = offset;
        auto base = base_span;
        while (base != std::nullopt) {
            orig_offset += base->prev_offset;
            orig_span = base->span;
            base = orig_span->base_span;
        }
        return std::make_pair(orig_span, orig_offset);
    }


    // Span from which this span is derived
    std::optional<SpanBase> base_span;

    // Should be set by container during layout
    float width;
    LineMetrics metrics;
};

};  // namespace aardvark::inline_layout
