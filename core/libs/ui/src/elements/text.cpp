#include "elements/text.hpp"

#include "SkPaint.h"
#include "inline_layout/utils.hpp"

namespace aardvark {

void paint_text_decoration(
    TextDecoration& decoration,
    SkCanvas& canvas,
    inline_layout::LineMetrics& metrics,
    float text_width) {
    auto paint = SkPaint();
    paint.setColor(decoration.color.to_sk_color());
    paint.setStrokeWidth(decoration.thickness);
    paint.setAntiAlias(true);
    auto top = 0.0;
    switch (decoration.kind) {
        case TextDecorationKind::overline:
            top = -(float)decoration.thickness / 2;
        break;
        case TextDecorationKind::line_through:
            top = metrics.baseline - metrics.x_height / 2;
        break;
        case TextDecorationKind::underline:
            top = metrics.baseline + (float)decoration.thickness / 2 + 2;
        break;
    }
    canvas.drawLine(0, top, text_width, top, paint);
}

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

    // TODO improve paint order
    for (auto& decoration : style.decorations) {
        paint_text_decoration(decoration, *layer->canvas, metrics, size.width);
    }
};

}  // namespace aardvark
