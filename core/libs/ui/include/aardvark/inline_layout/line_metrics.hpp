#pragma once

#include "SkFont.h"
#include "SkFontMetrics.h"

namespace aardvark::inline_layout {

struct LineMetrics {
    float height;
    float baseline;
    float x_height;

    LineMetrics add(float ascent, float descent);
    LineMetrics scale(float ratio);

    static LineMetrics from_font(const SkFont& font);
};

}  // namespace aardvark::inline_layout
