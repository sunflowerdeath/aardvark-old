#pragma once

#include <SkPaint.h>
#include <unicode/unistr.h>

#include <optional>

#include "../base_types.hpp"
#include "../element.hpp"
#include "span.hpp"

namespace aardvark::inline_layout {

float measure_text_width(const UnicodeString& text, const SkPaint& paint,
                         std::optional<int> num_chars = std::nullopt);

int break_text(const UnicodeString& text, const SkPaint& paint,
               float available_width, float* width = nullptr);

// Converts ICU UnicodeString to C++ std string (with UTF-16 encoding)
std::string icu_to_std_string(const UnicodeString& text);

// Calculates combined line metrics of the list of spans
LineMetrics calc_combined_metrics(
    const std::vector<std::shared_ptr<Span>>& spans,
    const LineMetrics& default_metrics);

// Renders list of spans into the provided container
void render_spans(const std::vector<std::shared_ptr<Span>>& spans,
                  const LineMetrics& metrics, const Position& offset,
                  std::vector<std::shared_ptr<Element>>* container);

SkPaint make_default_paint();

}  // namespace aardvark::inline_layout
