#include "elements/text.hpp"

#include "inline_layout/utils.hpp"

namespace aardvark {

float TextElement::get_intrinsic_height() {
    auto metrics = inline_layout::LineMetrics::from_paint(skpaint);
    return metrics.height;
}

float TextElement::get_intrinsic_width() {
    return inline_layout::measure_text_width(text, skpaint);
}

Size TextElement::layout(BoxConstraints constraints) {
    // TODO cache (lazy calculation)
    auto metrics = inline_layout::LineMetrics::from_paint(skpaint);
    return Size{inline_layout::measure_text_width(text, skpaint),
                metrics.height};
};

void TextElement::paint(bool is_changed) {
    auto layer = document->get_layer();
    document->setup_layer(layer, this);
    auto metrics = inline_layout::LineMetrics::from_paint(skpaint);
    layer->canvas->drawText(
        text.getBuffer(),   // text
        text.length() * 2,  // byteLength
        0,                  // x
        metrics.baseline,   // y
        skpaint             // paint
    );
};

}  // namespace aardvark
