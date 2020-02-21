#pragma once

#include <memory>
#include <unicode/unistr.h>
#include "SkPaint.h"
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"
#include "../inline_layout/line_metrics.hpp"

namespace aardvark::elements {

class Text : public Element {
  public:
    Text()
        : Element(/* is_repaint_boundary */ false,
                  /* size_depends_on_parent */ true){};

    Text(UnicodeString text, SkPaint paint, bool is_repaint_boundary = false)
        : Element(is_repaint_boundary, /* size_depends_on_parent */ true),
          text(text),
          skpaint(paint){};

    std::string get_debug_name() override { return "Text"; };
    float get_intrinsic_height() override;
    float get_intrinsic_width() override;
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;

    UnicodeString text = UnicodeString((UChar*)u"");
    SkPaint skpaint = Text::make_default_paint();

  private:
    static SkPaint make_default_paint() {
        SkPaint paint;
        paint.setColor(SK_ColorBLACK);
        paint.setTextSize(16);
        paint.setAntiAlias(true);
        paint.setTextEncoding(SkPaint::kUTF16_TextEncoding);
        return paint;
    };
};

}  // namespace aardvark::elements
