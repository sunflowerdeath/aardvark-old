#pragma once

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

    static InlineLayoutResult fit(float width, LineMetrics metrics);
    static InlineLayoutResult split(float width, LineMetrics metrics,
                                    std::shared_ptr<Span> fit_span,
                                    std::shared_ptr<Span> remainder_span);
    static InlineLayoutResult wrap();
};

struct SpanSelectionEdge {
    enum class Type {start, offset, end};
    Type type;
    int offset = 0;
};

struct SpanSelectionRange {
    SpanSelectionEdge from;
    SpanSelectionEdge to;
};

struct SpanBase {
    Span* span;
    int prev_offset;
};

// Span represents part of the contents of the inline container element
class Span {
  public:
    Span(std::optional<SpanBase> base_span = std::nullopt)
        : base_span(base_span){};

    virtual ~Span(){};

    // Defines how the content should be lay out by the container
    virtual InlineLayoutResult layout(InlineConstraints constraints){};

    // Returns elements that represent this span in the document
    virtual std::shared_ptr<Element> render(
        std::optional<SpanSelectionRange> selection){};

    // Calculates position of the span in the line, default is baseline
    virtual float vert_align(LineMetrics line, LineMetrics span) {
        return vert_align::baseline(line, span);
    };

    // Span from which this span is derived
    std::optional<SpanBase> base_span;

    // Should be set by container during layout
    float width;
    LineMetrics metrics;

    // To layout span you should use static method, because span can not return
    // shared pointer to itself
    static InlineLayoutResult layout(std::shared_ptr<Span> span_sp,
                                     InlineConstraints constraints);
};

};  // namespace aardvark::inline_layout
