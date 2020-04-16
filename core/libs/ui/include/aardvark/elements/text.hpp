#pragma once

#include <unicode/unistr.h>

#include <memory>

#include "../element.hpp"
#include "../inline_layout/line_metrics.hpp"
#include "SkPaint.h"

namespace aardvark {

class TextElement : public Element {
  public:
    TextElement()
        : Element(
              /* is_repaint_boundary */ false,
              /* size_depends_on_parent */ true){};

    TextElement(
        UnicodeString text, SkPaint paint, bool is_repaint_boundary = false)
        : Element(is_repaint_boundary, /* size_depends_on_parent */ true),
          text(text),
          skpaint(paint){};

    std::string get_debug_name() override { return "Text"; };
    float get_intrinsic_height() override;
    float get_intrinsic_width() override;
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;

    // TODO decide utf8/16
    void set_text(std::string& new_text) {
        text = UnicodeString(new_text.c_str());
        change();
    }

    std::string get_text() {
        std::string utf8;
        text.toUTF8String(utf8);
        return utf8;
    }

    UnicodeString text = UnicodeString((UChar*)u"");
    SkPaint skpaint = TextElement::make_default_paint();

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

}  // namespace aardvark
