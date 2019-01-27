#include "inline_layout.hpp"

namespace aardvark::inline_layout {

LineMetrics LineMetrics::add(int added) {
    return LineMetrics{height + added, baseline, x_height};
};

LineMetrics LineMetrics::scale(int ratio) {
    return LineMetrics{height * ratio, baseline, x_height};
}

LineMetrics LineMetrics::from_paint(const SkPaint& paint) {
    SkPaint::FontMetrics metrics;
    (void)paint.getFontMetrics(&metrics);
    return LineMetrics{
        -metrics.fAscent + metrics.fDescent,  // height
        -metrics.fAscent,                     // baseline
        metrics.fXHeight                      // x_height
    };
};

namespace vert_align {

float baseline(LineMetrics line, LineMetrics span) {
    return line.baseline - span.baseline;
};

float top(LineMetrics line, LineMetrics span) { return 0; };

float center(LineMetrics line, LineMetrics span) {
    return (line.height - span.height) / 2;
};

float bottom(LineMetrics line, LineMetrics span) {
    return line.height - span.height;
};

int text_center(LineMetrics line, LineMetrics span) {
    return line.baseline - line.x_height / 2 - span.height / 2;
};

}  // namespace vert_align

InlineLayoutResult InlineLayoutResult::fit(float width, LineMetrics metrics) {
    return InlineLayoutResult{Type::fit, width, metrics};
};

InlineLayoutResult InlineLayoutResult::split(
    float width, LineMetrics metrics, std::shared_ptr<Span> fit_span,
    std::shared_ptr<Span> remainder_span) {
    return InlineLayoutResult{Type::split, width, metrics, fit_span,
                              remainder_span};
};

InlineLayoutResult InlineLayoutResult::wrap() {
    return InlineLayoutResult{Type::wrap};
};

InlineLayoutResult Span::layout(std::shared_ptr<Span> span_sp,
                                InlineConstraints constraints) {
    auto result = span_sp->layout(constraints);
    if (result.type == InlineLayoutResult::Type::fit) {
        result.fit_span = span_sp;
    } else if (result.type == InlineLayoutResult::Type::wrap) {
        result.remainder_span = span_sp;
    }
    return result;
};

} // namespace aardvark::inline_layout
