#pragma once

#include <unicode/unistr.h>

#include <memory>

#include "../element.hpp"
#include "../inline_layout/utils.hpp"
#include "../inline_layout/line_metrics.hpp"
#include "SkPaint.h"
#include "SkFont.h"

namespace aardvark {

struct TextStyle {
    Color color = Color::from_sk_color(SK_ColorBLACK);
    std::string font_family;
    int font_size = 16;
    float line_height = 1;

    SkPaint to_sk_paint() {
        SkPaint paint;
        paint.setColor(color.to_sk_color());
        paint.setAntiAlias(true);
        return paint;
    }

    SkFont to_sk_font() {
        SkFont font;
        font.setSize(font_size);
        return font;
    }

    inline_layout::LineMetrics get_metrics() {
        return inline_layout::LineMetrics::from_sk_font(to_sk_font());
    }
};

class TextElement : public Element {
  public:
    TextElement()
        : Element(
              /* is_repaint_boundary */ false,
              /* size_depends_on_parent */ true){};

    TextElement(
        UnicodeString text, TextStyle style, bool is_repaint_boundary = false)
        : Element(is_repaint_boundary, /* size_depends_on_parent */ true),
          text(std::move(text)),
          style(std::move(style)){};

    std::string get_debug_name() override { return "Text"; };
    float get_intrinsic_height(float width) override;
    float get_intrinsic_width(float height) override;
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

    ELEMENT_PROP(TextStyle, style);
};

}  // namespace aardvark
