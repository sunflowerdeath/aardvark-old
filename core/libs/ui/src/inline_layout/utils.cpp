#include "inline_layout/utils.hpp"

#include "elements/aligned.hpp"
#include "elements/sized.hpp"
#include <iostream>

namespace aardvark::inline_layout {

float measure_text_width(
    const UnicodeString& text,
    const SkFont& font,
    std::optional<int> num_chars) {
    auto byte_length =
        (num_chars == std::nullopt ? text.length() : num_chars.value()) * 2;
    return font.measureText(
        text.getBuffer(), byte_length, SkTextEncoding::kUTF16);
};

std::vector<float> get_glyph_widths(
    const UnicodeString& text, const SkFont& font) {
    auto count = text.length(); 
    if (count == 0) return std::vector<float>();
    SkGlyphID glyphs[count]; // countChar32
    auto glyph_count = font.textToGlyphs(
        text.getBuffer(),        // text
        count * 2,               // byteLength
        SkTextEncoding::kUTF16,  // encoding
        glyphs,                  // glyphs
        count                    // maxGlyphCount
    );
    auto widths = std::vector<float>(glyph_count);
    font.getWidths(glyphs, glyph_count, widths.data());
    return widths;
}

int find_break_position(
    float max_width, std::vector<float> glyph_widths, float* measured_width) {
    auto measured_count = 0;
    *measured_width = 0;
    for (int i = 0; i < glyph_widths.size(); i++) {
        if (*measured_width + glyph_widths[i] / 2 > max_width) break;
        measured_count++;
        *measured_width += glyph_widths[i];
    }
    return measured_count;
}

int break_text(
    const UnicodeString& text,
    const SkFont& font,
    float max_width,
    float* measured_width) {
    auto count = text.length();
    if (count == 0) {
        *measured_width = 0;
        return 0;
    }
    
    // TODO only once per text span
    SkGlyphID glyphs[count]; // countChar32
    auto glyph_count = font.textToGlyphs(
        text.getBuffer(),        // text
        count * 2,               // byteLength
        SkTextEncoding::kUTF16,  // encoding
        glyphs,                  // glyphs
        count                    // maxGlyphCount
    );
    float widths[glyph_count];
    font.getWidths(glyphs, glyph_count, widths);
    
    auto measured_count = 0;
    *measured_width = 0;
    for (int i = 0; i < glyph_count; i++) {
        if (*measured_width + widths[i] > max_width) break;
        measured_count++;
        *measured_width += widths[i];
    }
    return measured_count;
};

std::string icu_to_std_string(const UnicodeString& text) {
    std::string std_string;
    text.toUTF8String(std_string);
    return std_string;
};

LineMetrics calc_combined_metrics(
    const std::vector<std::shared_ptr<Span>>& spans,
    const LineMetrics& default_metrics) {
    float max_ascent = default_metrics.baseline;
    float max_descent = default_metrics.height - default_metrics.baseline;
    for (auto& span : spans) {
        if (span->metrics.baseline > max_ascent) {
            max_ascent = span->metrics.baseline;
        }
        auto descent = span->metrics.height - span->metrics.baseline;
        if (descent > max_descent) max_descent = descent;
    }
    return LineMetrics{
        max_ascent + max_descent,  // height
        max_ascent,                // baseline
        default_metrics.x_height   // x_height
    };
}

void render_spans(
    const std::vector<std::shared_ptr<Span>>& spans,
    const LineMetrics& metrics,
    const Position& offset,
    std::vector<std::shared_ptr<Element>>* container,
    Element* parent) {
    auto current_width = 0.0f;
    for (auto& span : spans) {
        auto elem = span->render();
        auto sizeConstraints = SizeConstraints{
            Value::abs(span->width), Value::abs(span->metrics.height)};
        auto align = Alignment::top_left(
            Value::abs(span->vert_align(metrics, span->metrics) + offset.top),
            Value::abs(current_width + offset.left));
        auto aligned = std::make_shared<AlignedElement>(
            std::make_shared<SizedElement>(elem, sizeConstraints), align);
        aligned->parent = parent;
        container->emplace_back(aligned);
        current_width += span->width;
    }
}

SkPaint make_default_paint() {
    SkPaint paint;
    paint.setColor(SK_ColorBLACK);
    paint.setAntiAlias(true);
    return paint;
}

SkFont make_default_font() {
    SkFont font;
    font.setSize(16);
    return font;
}

}  // namespace aardvark::inline_layout
