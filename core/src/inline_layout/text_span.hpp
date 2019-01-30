#pragma once

#include <optional>
#include <memory>
#include <unicode/brkiter.h>
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
    std::shared_ptr<Element> render(
        std::optional<SpanSelectionRange> selection) override;
    UnicodeString text;
    SkPaint paint;
    LineBreak linebreak;

  private:
    BreakIterator* linebreaker;
    InlineLayoutResult split(int pos, float measured_width);
};

}  // namespace aardvark::inline_layout

