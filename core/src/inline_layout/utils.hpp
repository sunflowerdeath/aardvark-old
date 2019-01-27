#pragma once

#include <unicode/unistr.h>
#include "../base_types.hpp"
#include "../elements/elements.hpp"
#include "span.hpp"

namespace aardvark::inline_layout {

float measure_text_width(const UnicodeString& text, const SkPaint& paint);

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

}  // namespace aardvark::inline_layout
