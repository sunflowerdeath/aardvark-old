#include "inline_layout.hpp"

namespace aardvark::inline_layout {

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

} // namespace aardvark::inline_layout
