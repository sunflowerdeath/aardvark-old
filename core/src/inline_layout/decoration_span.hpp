#pragma once

#include <optional>
#include <memory>
#include "../element.hpp"
#include "../elements/border.hpp"
#include "../elements/background.hpp"
#include "inline_layout.hpp"
#include "utils.hpp"

namespace aardvark::inline_layout {

struct Decoration {
    std::optional<SkColor> background;
    std::optional<BoxBorders> borders;
    std::optional<EdgeInsets> insets;

    // Returns new decoration with only left part
    Decoration left();
 
    // Returns new decoration with only right part
    Decoration right();
};

class DecorationSpan : public Span {
  public:
    DecorationSpan(std::vector<std::shared_ptr<Span>> content,
                   Decoration decoration,
                   std::optional<SpanBase> base_span = std::nullopt);
    InlineLayoutResult layout(InlineConstraints constraints) override;
    std::shared_ptr<Element> render(
        std::optional<SpanSelection> selection) override;
    std::vector<std::shared_ptr<Span>> content;
    Decoration decoration;

  public:
    std::unique_ptr<Span> fit_span;
    std::unique_ptr<Span> remainder_span;
};

}  // namespace aardvark::inline_layout

