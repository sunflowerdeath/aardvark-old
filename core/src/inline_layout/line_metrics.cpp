#include "line_metrics.hpp"

namespace aardvark::inline_layout {

LineMetrics LineMetrics::add(float added) {
    return LineMetrics{height + added, baseline, x_height};
};

LineMetrics LineMetrics::scale(float ratio) {
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

}  // namespace aardvark::inline_layout
