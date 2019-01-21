#pragma once

namespace aardvark {

struct LineMetrics {
    float height;
    float x_height;
    float baseline;
};

// Span is aligned on the baseline
float baseline(LineMetrics line, LineMetrics span) {
    return line.baseline - span.baseline;
};

// Span is aligned at the top of the line
float top(LineMetrics line, LineMetrics span) { return 0; };

// Span is aligned at the bottom of the line
float bottom(LineMetrics line, LineMetrics span) {
    return line.height - span.height;
};

// Middle of the span is aligned with middle of x-height of the line
float middle(LineMetrics line, LineMetrics span) {
    return line.baseline - line.x_height / 2 - span.height / 2;
};

}
