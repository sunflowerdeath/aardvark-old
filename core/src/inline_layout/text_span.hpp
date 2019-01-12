#include <optional>

#include <unicode/brkiter.h>
#include "inline_layout.hpp"

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

int measure_text(UnicodeString text, SkPaint& paint);

class TextSpan : public Span {
  public:
    TextSpan(UnicodeString text, SkPaint paint,
             std::optional<SpanBase> base_span = std::nullopt);
    InlineLayoutResult layout(InlineConstraints constraints) override;
    UnicodeString text;
    SkPaint paint;
  private:
    BreakIterator* linebreaker;
    std::shared_ptr<TextSpan> fit_span;
    std::shared_ptr<TextSpan> remainder_span;
};

}  // namespace aardvark::inline_layout

