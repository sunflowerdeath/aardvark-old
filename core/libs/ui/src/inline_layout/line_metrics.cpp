#include "inline_layout/line_metrics.hpp"

namespace aardvark::inline_layout {

LineMetrics LineMetrics::add(float ascent, float descent) {
    return LineMetrics{height + ascent + descent, baseline + descent, x_height};
};

LineMetrics LineMetrics::scale(float ratio) {
    if (ratio == 1) return *this;
    auto d = height * (ratio - 1);
    return LineMetrics{height + d, baseline + d / 2, x_height};
}

LineMetrics LineMetrics::from_sk_font(const SkFont& font) {
    SkFontMetrics metrics;
    (void)font.getMetrics(&metrics);
    return LineMetrics{
        -metrics.fAscent + metrics.fDescent,  // height
        -metrics.fAscent,                     // baseline
        metrics.fXHeight                      // x_height
    };
};

}  // namespace aardvark::inline_layout
