#pragma once

#include "SkPaint.h"

namespace aardvark::inline_layout {

struct LineMetrics {
    float height;
    float baseline;
    float x_height;

    LineMetrics add(int added);
    LineMetrics scale(int ratio);

    static LineMetrics from_paint(const SkPaint& paint);
};

}  // namespace aardvark::inline_layout
