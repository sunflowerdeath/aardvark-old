#pragma once

#include <unicode/unistr.h>

#include <memory>

#include "../element.hpp"
#include "../inline_layout/utils.hpp"
#include "../inline_layout/line_metrics.hpp"
#include "SkPaint.h"
#include "SkFont.h"

namespace aardvark {

class TextElement : public Element {
  public:
    TextElement()
        : Element(
              /* is_repaint_boundary */ false,
              /* size_depends_on_parent */ true){};

    TextElement(
        UnicodeString text,
        SkPaint paint,
        SkFont font,
        bool is_repaint_boundary = false)
        : Element(is_repaint_boundary, /* size_depends_on_parent */ true),
          text(std::move(text)),
          skpaint(std::move(paint)),
          skfont(std::move(font)){};

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
    SkPaint skpaint = inline_layout::make_default_paint();
    SkFont skfont = inline_layout::make_default_font();
};

}  // namespace aardvark
