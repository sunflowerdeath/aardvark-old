#pragma once

#include <optional>
#include <memory>
#include <unicode/brkiter.h>
#include "../element.hpp"
#include "../elements/text.hpp"
#include "inline_layout.hpp"
#include "utils.hpp"

namespace aardvark::inline_layout {

class TextSpan : public Span {
  public:
    TextSpan(UnicodeString text, SkPaint paint,
             std::optional<SpanBase> base_span = std::nullopt);
    InlineLayoutResult layout(InlineConstraints constraints) override;
    std::shared_ptr<Element> render(
        std::optional<SpanSelectionRange> selection) override;
    UnicodeString text;
    SkPaint paint;

  private:
    BreakIterator* linebreaker;
};

}  // namespace aardvark::inline_layout

