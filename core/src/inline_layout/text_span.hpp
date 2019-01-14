#include <optional>
#include <unicode/brkiter.h>
#include "inline_layout.hpp"

namespace aardvark::inline_layout {

class TextSpan : public Span {
  public:
    TextSpan(UnicodeString text, SkPaint paint,
             std::optional<SpanBase> base_span = std::nullopt);
    InlineLayoutResult layout(InlineConstraints constraints) override;
    UnicodeString text;
    SkPaint paint;
  private:
    BreakIterator* linebreaker;
    std::unique_ptr<TextSpan> fit_span;
    std::unique_ptr<TextSpan> remainder_span;
};

}  // namespace aardvark::inline_layout

