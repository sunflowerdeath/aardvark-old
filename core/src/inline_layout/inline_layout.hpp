#pragma once

#include <memory>
#include <optional>
#include <variant>
#include <unicode/unistr.h>
#include "../base_types.hpp"
#include "../element.hpp"
#include "SkPaint.h"

namespace aardvark::inline_layout {

enum class LineBreak {
    // Use default unicode line breaking algorithm
    normal,

    // Never allow break text
    never,

    // Line break is allowed between any two characters
    anywhere,

    // If word can not fit in the line using default breaking algorithm, it is
    // allowed to break it at arbitrary point
    overflow
};

class Span;

struct InlineConstraints {
    float remaining_line_width;
    float total_line_width;
    float padding_before;
    float padding_after;
};

struct LineMetrics {
    float height;
    float baseline;
    float x_height;

    LineMetrics add(int added) {
        return LineMetrics{height + added, baseline, x_height};
    };

    LineMetrics scale(int ratio) {
        return LineMetrics{height * ratio, baseline, x_height};
    };

    static LineMetrics from_paint(const SkPaint& paint);
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

    static InlineLayoutResult fit(float width, LineMetrics metrics) {
        return InlineLayoutResult{Type::fit, width, metrics};
    };

    static InlineLayoutResult split(float width, LineMetrics metrics,
                                    std::shared_ptr<Span> fit_span,
                                    std::shared_ptr<Span> remainder_span) {
        return InlineLayoutResult{Type::split, width, metrics, fit_span,
                                  remainder_span};
    };

    static InlineLayoutResult wrap() { return InlineLayoutResult{Type::wrap}; };
};

// Representation of the selected range in the inline container element
struct Selection {
    Span* base;
    int base_offset;
    Span* extent;
    int extent_offset;
};

struct s_start {};
struct s_end {};
struct s_offset {
    s_offset(int pos) : pos(pos){};
    int pos;
};

// Selected range in the span object
using SpanSelection = std::variant<s_start, s_end, s_offset>;

struct SpanBase {
    Span* span;
    int prev_offset;
};

// Span represents part of the contents of the inline container element
class Span {
  public:
    Span(std::optional<SpanBase> base_span = std::nullopt){};

    // Defines how the content should be lay out by the container
    virtual InlineLayoutResult layout(InlineConstraints constraints){};

    // Returns elements that represent this span in the document
    virtual std::shared_ptr<Element> render(
        std::optional<SpanSelection> selection){};

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
    // shared pointer to self
    static InlineLayoutResult layout(std::shared_ptr<Span> span_sp,
                                     InlineConstraints constraints) {
        auto result = span_sp->layout(constraints);
        if (result.type == InlineLayoutResult::Type::fit) {
            result.fit_span = span_sp;
        } else if (result.type == InlineLayoutResult::Type::wrap) {
            result.remainder_span = span_sp;
        }
        return result;
    };
};

};  // namespace aardvark::inline_layout
