#pragma once

#include "SkPaint.h"

namespace aardvark::inline_layout {

struct LineMetrics {
    float height;
    float baseline;
    float x_height;

    LineMetrics add(float added);
    LineMetrics scale(float ratio);

    static LineMetrics from_paint(const SkPaint& paint);
};

}  // namespace aardvark::inline_layout
