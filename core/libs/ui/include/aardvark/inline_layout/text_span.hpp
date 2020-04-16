#pragma once

#include <unicode/brkiter.h>

#include <memory>
#include <optional>

#include "../element.hpp"
#include "../elements/text.hpp"
#include "span.hpp"
#include "utils.hpp"

namespace aardvark::inline_layout {

enum class LineBreak {
    // Use default unicode line breaking algorithm
    normal,

    // Never allow break text
    never,

    // Line break is allowed between any two characters
    anywhere,

    // If word can not fit in the line using default breaking algorithm, it is
    // allowed to break it at arbitrary point
    overflow
};

class TextSpan : public Span {
  public:
    TextSpan(UnicodeString text, SkPaint paint,
             LineBreak linebreak = LineBreak::normal,
             std::optional<SpanBase> base_span = std::nullopt);
    ~TextSpan() override;

    InlineLayoutResult layout(InlineConstraints constraints) override;
    std::shared_ptr<Element> render() override;
    UnicodeString get_text() override;
    int get_text_length() override;
    std::shared_ptr<Span> slice(int start, int end) override;
    int get_text_offset_at_position(int position) override;

    UnicodeString text;
    NODE_PROP_DEFAULT(SkPaint, paint, make_default_paint());
    NODE_PROP_DEFAULT(LineBreak, linebreak, LineBreak::normal);

    // TODO decide utf8/16
    void set_text(std::string& new_text) {
        text = UnicodeString(new_text.c_str());
        change();
    }

    std::string get_text_p() { // TODO
        std::string utf8;
        text.toUTF8String(utf8);
        return utf8;
    }

  private:
    BreakIterator* linebreaker;
    InlineLayoutResult split(int pos, float measured_width);
    InlineLayoutResult fit(float measured_width);
    InlineLayoutResult wrap();
    InlineLayoutResult break_segment(const UnicodeString& text,
                                     const InlineConstraints& constraints,
                                     bool is_last_segment);
};

}  // namespace aardvark::inline_layout

