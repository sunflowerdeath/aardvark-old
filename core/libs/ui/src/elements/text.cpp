#include "elements/text.hpp"

#include "inline_layout/utils.hpp"

namespace aardvark {

float TextElement::get_intrinsic_height(float width) {
    auto metrics = inline_layout::LineMetrics::from_font(skfont);
    return metrics.height;
}

float TextElement::get_intrinsic_width(float height) {
    return inline_layout::measure_text_width(text, skfont);
}

Size TextElement::layout(BoxConstraints constraints) {
    // TODO cache (lazy calculation)
    auto metrics = inline_layout::LineMetrics::from_font(skfont);
    return Size{
        inline_layout::measure_text_width(text, skfont), metrics.height};
};

void TextElement::paint(bool is_changed) {
    auto layer = document->get_layer();
    document->setup_layer(layer, this);
    auto metrics = inline_layout::LineMetrics::from_font(skfont);
    layer->canvas->drawSimpleText(
        text.getBuffer(),        // text
        text.length() * 2,       // byteLength
        SkTextEncoding::kUTF16,  // encoding
        0,                       // x
        metrics.baseline,        // y
        skfont,                  // font
        skpaint                  // paint
    );
    // layer->canvas->drawText(
        // text.getBuffer(),   // text
        // text.length() * 2,  // byteLength
        // 0,                  // x
        // metrics.baseline,   // y
        // skpaint             // paint
    // );
};

}  // namespace aardvark
