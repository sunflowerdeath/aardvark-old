#include "text.hpp"

namespace aardvark::elements {

Text::Text(UnicodeString text, SkPaint paint, bool is_repaint_boundary)
    : Element(is_repaint_boundary, /* size_depends_on_parent */ true),
      text(text),
      skpaint(paint){};

Size Text::layout(BoxConstraints constraints) {
    return constraints.max_size();
};

void Text::paint(bool is_changed) {
    auto layer = document->get_layer();
    document->setup_layer(layer, this);
    auto metrics = inline_layout::LineMetrics::from_paint(skpaint);
    layer->canvas->drawText(text.getBuffer(),   // text
                            text.length() * 2,  // byteLength
                            0,                  // x
                            metrics.baseline,   // y
                            skpaint             // paint
    );
};

}  // namespace aardvark::elements
