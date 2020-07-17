#include "elements/text.hpp"

#include "inline_layout/utils.hpp"

namespace aardvark {

float TextElement::get_intrinsic_height(float width) {
    return style.get_metrics().height;
}

float TextElement::get_intrinsic_width(float height) {
    return inline_layout::measure_text_width(text, style.to_sk_font());
}

Size TextElement::layout(BoxConstraints constraints) {
    // TODO cache (lazy calculation)
    auto font = style.to_sk_font();
    auto metrics = inline_layout::LineMetrics::from_sk_font(font);
    return Size{inline_layout::measure_text_width(text, font), metrics.height};
};

void TextElement::paint(bool is_changed) {
    auto layer = document->get_layer();
    document->setup_layer(layer, this);

    auto paint = style.to_sk_paint();
    auto font = style.to_sk_font();
    auto metrics =
        inline_layout::LineMetrics::from_sk_font(font).scale(style.line_height);
    layer->canvas->drawSimpleText(
        text.getBuffer(),        // text
        text.length() * 2,       // byteLength
        SkTextEncoding::kUTF16,  // encoding
        0,                       // x
        metrics.baseline,        // y
        font,                    // font
        paint                    // paint
    );
};

}  // namespace aardvark
